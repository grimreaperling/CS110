/**
 * File: non-blocking-alphabet-client.cc
 * ---------------------------------
 * This program uses traditional networking and 
 * low-level (but otherwise traditional) I/O to 
 * ingest the entire alphabet in from the
 * slow-alphabet-server.
 */

#include <fcntl.h>    // for open
#include <string>     // for C++ string class
#include <iostream>   // for cout, cerr, endl
#include <cassert>    // for assert macro
#include <unistd.h>   // for close
#include "client-socket.h"
#include "non-blocking-utils.h"
using namespace std;

static const unsigned short kSlowAlphabetServerPort = 41411;
int main(int argc, char *argv[]) {
  int client = createClientSocket("localhost", kSlowAlphabetServerPort);
  assert(client != -1);
  assert(isBlocking(client));
  setAsNonBlocking(client);
  assert(isNonBlocking(client));
  
  size_t numReads = 0;
  size_t numSuccessfulReads = 0;
  size_t numUnsuccessfulReads = 0;
  size_t numBytes = 0;
  while (true) {
    char ch;
    ssize_t count = read(client, &ch, 1);
    if (count == 0) break; // we are truly done
    numReads++;
    if (count > 0) {
      assert(count == 1);
      numSuccessfulReads++;
      numBytes++;
      cout << ch << flush;
    } else {
      numUnsuccessfulReads++;
      assert(errno == EWOULDBLOCK || errno == EAGAIN);
    }
  }
  
  close(client);
  cout << endl;
  cout << "Alphabet Length: " << numBytes << " bytes." << endl;
  cout << "Num reads: " << numReads << " (" << numSuccessfulReads << " successful, " << numUnsuccessfulReads << " unsuccessful)." << endl;
  return 0;
}
