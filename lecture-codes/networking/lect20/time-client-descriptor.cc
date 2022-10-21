/**
 * File: time-client-descriptor.cc
 * --------------------
 * Implements a client program that connects to a specified time server and prints out the data that the
 * server sends back.  The program requires 2 command line arguments, the first being the server hostname
 * and the second being the server port number.
 * 
 * This program uses read() to read the data sent back from the server.
 */

#include <iostream>
#include <unistd.h>
#include "client-socket.h"
#include "socket++/sockstream.h"

using namespace std;

// The error code returned if we couldn't connect to the specified server
static const int kTimeServerInaccessible = 1;

// The error code returned if the user doesn't specify the server hostname / port
static const int kWrongArgumentCount = 1;


int main(int argc, char *argv[]) {
  if (argc != 3) {
      cerr << "Usage:\n\t" << argv[0] << " server port" << endl;
      cerr << "e.g.,\n\t" << argv[0] << " myth61.stanford.edu 12345" << endl;
          
      return kWrongArgumentCount;
  }

  // Open a connection to the server
  int socketDescriptor = createClientSocket(argv[1], atoi(argv[2]));

  if (socketDescriptor == kClientSocketError) {
    cerr << "Time server could not be reached" << "." << endl;
    cerr << "Aborting" << endl;
    return kTimeServerInaccessible;
  }

  // Read in the data from the server (assumed to be at most 1024 byte string)
  char buf[1024];
  size_t bytes_read = 0;
  while (true) {
    size_t read_this_time = read(socketDescriptor, buf + bytes_read, sizeof(buf) - bytes_read);
    if (read_this_time == 0) break;
    bytes_read += read_this_time;
  }
  buf[bytes_read] = '\0';

  close(socketDescriptor);

  cout << buf << flush;

  return 0;
}
