/**
 * File: web-get.cc
 * ----------------
 * This file provides a program that imitates 
 * the functionality of the wget built-in, which
 * pulls the remote document whose URL is specified via the
 * program's only argument.  It does this by sending an HTTP GET
 * request to the specified URL, parsing the HTTP response, and saving
 * the response payload to a file.
 */

#include <iostream>               // for cout, cerr, cout
#include <string>                 // for string
#include <fstream>                // for ofstream
#include "socket++/sockstream.h"  // for sockbuf, iosockstream
#include "client-socket.h"        // for createClientSocket
#include "string-utils.h"         // for startsWith
using namespace std;


// The error code returned if the user doesn't specify the URL
static const int kWrongArgumentCount = 1;

// The port we assume to contact for the HTTP request we make
static const unsigned short kDefaultHTTPPort = 80;

// The URL protocol we assume to use
static const string kProtocolPrefix = "http://";

// The buffer size in bytes to use when reading in chunks of the response payload
static const size_t kBufferSizeBytes = 1024;


/* This function splits a given URL into two components, the hostname
 * and the path.  E.g. if the url is web.stanford.edu/class/cs110, the hostname
 * would be web.stanford.edu (all that is before the first /) and the path
 * would be /class/cs110 (all that is after the first /).  If the URL starts
 * with http://, that is removed.  If there is no path component (eg.
 * www.google.com) then the path is specified as "/".
 */
static pair<string, string> parseURL(string url) {
  // If the URL starts with the protocol e.g. http://, remove it
  if (startsWith(url, kProtocolPrefix)) {
    url = url.substr(kProtocolPrefix.size());
  }

  // Search for the first /
  size_t found = url.find('/');

  // If there is none, the path should be /
  if (found == string::npos) return make_pair(url, "/");

  // Otherwise, the host is what is before the /, and the path is after the /
  string host = url.substr(0, found);
  string path = url.substr(found);
  return make_pair(host, path);
}

/* This function reads in bytes from the socketStream, which is assumed to
 * be just the payload of an HTTP response, until it reaches the end of available
 * data.  As it reads in the data, it writes it to a file with the specified name.
 */
static void readAndSavePayload(iosockstream& socketStream, const string& filename) {
  ofstream output(filename, ios::binary); // don't assume it's text
  size_t totalBytes = 0;
  while (!socketStream.fail()) {
    char buffer[kBufferSizeBytes] = {'\0'};
    socketStream.read(buffer, sizeof(buffer));
    totalBytes += socketStream.gcount();
    output.write(buffer, socketStream.gcount());
  }
  cout << "Total number of bytes fetched: " << totalBytes << endl;
}

/* This function reads the HTTP response sent from the server via the given
 * iosockstream.  It skips the status line and headers (we don't need them), 
 * reads in the payload, and saves it to a file with the given filename.
 */
static void readResponse(iosockstream& socketStream, const string& filename) {
  // Skip the status line and headers (we don't need any information from them)
  while (true) {
    string line;
    getline(socketStream, line);
    if (line.empty() || line == "\r") break;
  }

  readAndSavePayload(socketStream, filename);
}

/* This function takes a path and returns the last component of it (the
 * part after the last /).  If there is no / or if it ends in a /, this
 * function returns "index.html".
 */
static string getFileName(const string& path) {
  if (path.empty() || path[path.size() - 1] == '/') return "index.html";
  size_t found = path.rfind('/');
  return path.substr(found + 1);
}

/* This function sends an HTTP request to the specified host for the resource
 * at the specified path, reads the response, and saves the payload data in
 * the response to a file with the same name as the path requested.
 */
static void fetchContent(const string& host, const string& path) {
  // Create a connection to the server on the HTTP port
  int socketDescriptor = createClientSocket(host, kDefaultHTTPPort);
  if (socketDescriptor == kClientSocketError) {
    cerr << "Count not connect to host named \"" << host << "\"." << endl;
    return;
  }

  sockbuf socketBuffer(socketDescriptor);
  iosockstream socketStream(&socketBuffer);

  // Send our request (using HTTP/1.0 for simpler requests)
  socketStream << "GET " << path << " HTTP/1.0\r\n";
  socketStream << "Host: " << host << "\r\n";
  socketStream << "\r\n" << flush;

  // Read the server's response and save it to file
  readResponse(socketStream, getFileName(path));
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " <url>" << endl;
    return kWrongArgumentCount;
  }
  
  pair<string, string> hostAndPath = parseURL(argv[1]);
  fetchContent(hostAndPath.first, hostAndPath.second);
  return 0;
}
