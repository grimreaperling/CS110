/**
 * File: time-server-concurrent.cc
 * ----------------------------
 * This is a server program that concurrently handles incoming requests and sends back a single string
 * containing the current date/time to the client.  It uses the socket++ sockbuf and iosockstream
 * types to more easily write data back to a client, and a thread pool to handle incoming client
 * connections in parallel.
 */

#include <iostream>                // for cout, cett, endl
#include <ctime>                   // for time, gmtime, strftim
#include <sys/socket.h>            // for socket, bind, accept, listen, etc.
#include <climits>                 // for USHRT_MAX
#include <unistd.h>                // for close
#include "socket++/sockstream.h"   // for sockbuf, iosockstream
#include "server-socket.h"
#include "thread-pool-release.h"
#include "thread-utils.h"

using release::ThreadPool;
using namespace std;

// The error code returned if the user doesn't specify the server hostname / port
static const int kWrongArgumentCount = 1;

// The error code returned if we couldn't start a server on the specified port
static const int kServerStartFailure = 2;

// The number of threads we should have in our thread pool for concurrently handling requests
static const int kNumThreads = 4;

// This function returns a string representation of the current date and time.  Thread safe.
static string getCurrentDateTime() {
  //sleep_for(2000);
  time_t rawtime;
  time(&rawtime);
  struct tm tm;
  gmtime_r(&rawtime, &tm);
  char timestr[128]; // more than big enough
  /* size_t len = */ strftime(timestr, sizeof(timestr), "%c", &tm);
  return timestr;
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
      // Make a string of the current date and time and send it to the client
      string dateTime = getCurrentDateTime();
      sockbuf socketBuffer(clientDescriptor); // destructor closes socket
      iosockstream socketStream(&socketBuffer);
      socketStream << dateTime << endl;
    });
  }

  close(serverSocket);

  return 0;
}
