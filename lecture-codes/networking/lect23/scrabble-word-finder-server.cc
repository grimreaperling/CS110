/**
 * File: scrabble-word-finder-server.cc
 * ------------------------------------
 * This is an HTTP server program that concurrently handles incoming requests to
 * either fetch the scrabble words HTML page or find all valid scrabble words 
 * that can be created with specified letters.
 * It accepts HTTP requests and assumes that the requested path will be either "/"
 * or /words?letters=XXXXX.  The response payload in the HTTP
 * response is in either HTML (for the homepage) or JSON format ("Javascript Object Notation")
 * for the list of possible words given letters.  JSON is a common format for specifying 
 * non-HTML data that can be easily parsed by code to use in programs.  
 * The JSON payload returned would look something like this:
 *
 * {
 *   possibilities: ["word1", "word2", "word3"]
 * }
 *
 */

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <sys/wait.h>
#include <ext/stdio_filebuf.h>
#include "server-socket.h"
#include "scrabble-word-finder.h"
#include "socket++/sockstream.h"
#include "subprocess.h"
#include "thread-pool-release.h"

using namespace std;
using namespace __gnu_cxx; // __gnu_cxx::stdio_filebuf -> stdio_filebuf 
using release::ThreadPool;

// The error code returned if the user doesn't specify the server hostname / port
static const int kWrongArgumentCount = 1;

// The error code returned if we couldn't start a server on the specified port
static const int kServerStartFailure = 2;

// The number of threads we should have in our thread pool for concurrently handling requests
static const int kNumThreads = 8;


/* This function takes the given array of words and returns the JSON payload
 * version of it we can send back to the client.  JSON stands for
 * "Javascript Object Notation".  It is a common format for specifying non-HTML
 * data that can be easily parsed by code to use in programs.  The payload
 * string returned by this function would look something like this:
 *
 * {
 *   "possibilities": ["word1", "word2", "word3"]
 * }
 *
 */
static string constructJSONPayload(const vector<string>& possibilities) {
  /* Use an ostringstream to construct the payload string.
   * An ostringstream is like cout, but it doesn't print to the screen;
   * when you're done adding to the stream, you can convert it to a
   * string.
   */
  ostringstream payload;
  payload << "{" << endl;
  payload << "  \"possibilities\": [";

  // Append each word, followed by a comma for all but the last word
  for (size_t i = 0 ; i < possibilities.size(); i++) {
    payload << "\"" << possibilities[i] << "\"";
    if (i < possibilities.size() - 1) payload << ", ";
  }

  payload << "]" << endl << "}" << endl;
  return payload.str();
}

/* This function constructs an HTTP response to send the specified payload
 * back to the requester via the specified stream.  The payload is assumed
 * to be the specified content type, and that type is also included as a header.
 */
static void sendResponse(iosockstream& socketStream, const string& payload, const string& contentType) {
  socketStream << "HTTP/1.1 200 OK\r\n";
  socketStream << "Content-Type: " << contentType << "\r\n";
  socketStream << "Content-Length: " << payload.size() << "\r\n";
  socketStream << "\r\n";
  socketStream << payload << flush;
}


/* This function fills in the specified vector with words formable with the
 * specified letters.  It does this by spawning a child process with subprocess()
 * that runs the scrabble-word-finder program, which outputs all the words that
 * can be formed with letters specified as a command-line argument.  This function
 * returns once that process finishes.
 */
static void findFormableWords(const string& letters, vector<string>& formableWords) {
  // Make an argument array for the command subprocess should run
  const char *command[] = {"./scrabble-word-finder", letters.c_str(), NULL};
  subprocess_t sp = subprocess(const_cast<char **>(command), false, true);

  // Make a stream around the file descriptor so we can read lines with getline
  stdio_filebuf<char> inbuf(sp.ingestfd, ios::in);
  istream instream(&inbuf);
  while (true) {
    // Read the next line and add it to the list of formable words
    string word;
    getline(instream, word);
    if (instream.fail()) break;
    formableWords.push_back(word);
  }
  // Make sure to only return from this function once the process has finished
  waitpid(sp.pid, NULL, 0);
}

/* This function reads in the request sent via the specified stream
 * and sends back the appropriate response.  If the request was for the "/" path,
 * we send back the HTML file scrabble-word-finder.html.  Otherwise, we assume
 * that the request path is a list of scrabble letters, and we send back a
 * list of valid words using those letters.  The list is formatted in JSON
 * "Javascript Object Notation" format.  Any headers in the request are
 * ignored.
 */
static void handleRequest(iosockstream& socketStream) {
  string method;      // e.g. GET
  string path;        // e.g. /letters
  string protocol;    // e.g. HTTP/1.0

  /* We can use stream operators to tokenize; it reads each space-separated
   * chunk into each specified variable.
   */
  socketStream >> method >> path >> protocol;

  // Extract just the query params, e.g. "key=value" in "/url?key=value"
  size_t queryParamsStart = path.find("?");
  string queryParams = "";
  if (queryParamsStart != string::npos) {
    queryParams = path.substr(queryParamsStart + 1);
    path = path.substr(0, queryParamsStart);
  }

  // read in the rest of the line, though we don't need it for anything
  string newline;
  getline(socketStream, newline);

  // Skip over the headers that follow, we don't need them either
  while (true) {
    string line;
    getline(socketStream, line);
    if (line.empty() || line == "\r") break;
  }

  // The payload and its format differ depending on what was requested
  string payload;
  string contentType;

  if (path == "/") {
    // send back HTML file
    ifstream fileStream("scrabble-word-finder.html");
    std::stringstream fileStringStream;
    fileStringStream << fileStream.rdbuf();
    payload = fileStringStream.str();
    contentType = "text/html; charset=UTF-8";
  } else if (path == "/words" && queryParams.find("letters=") != string::npos) {
    // compute valid words with these letters and send them back as JSON
    string letters = queryParams.substr(queryParams.find("letters=") + string("letters=").length());
    sort(letters.begin(), letters.end());
    vector<string> formableWords;
    findFormableWords(letters, formableWords);
    payload = constructJSONPayload(formableWords);
    contentType = "text/javascript; charset=UTF-8";
  } else {
    // Unknown request type
    payload = "<h1>Error: invalid request for resource \"" + path + "\"</h1>\n";
    contentType = "text/html; charset=UTF-8";
  }

  sendResponse(socketStream, payload, contentType);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    cerr << "Usage:\n\t" << argv[0] << " port" << endl;
    cerr << "e.g.,\n\t" << argv[0] << " 12345" << endl;

    return kWrongArgumentCount;
  }

  // Create a server socket we can use to listen for incoming requests
  unsigned short port = atoi(argv[1]);
  int serverSocket = createServerSocket(port);
  if (serverSocket == kServerSocketFailure) {
    cerr << "Error: Could not start server on port " << port << "." << endl;
    cerr << "Aborting... " << endl;
    return kServerStartFailure;
  }
  
  cout << "Server listening on port " << port << "." << endl;

  // Create a thread pool to concurrently handle client requests
  ThreadPool pool(kNumThreads);

  while (true) {
    // Wait for an incoming client connection and establish a descriptor for it
    int clientDescriptor = accept(serverSocket, NULL, NULL);

    cout << "Got incoming request!" << endl;

    pool.schedule([clientDescriptor]() {
      sockbuf socketBuffer(clientDescriptor); // destructor closes socket
      iosockstream socketStream(&socketBuffer);
      handleRequest(socketStream);
    });
  }

  close(serverSocket);
  
  return 0;
}
