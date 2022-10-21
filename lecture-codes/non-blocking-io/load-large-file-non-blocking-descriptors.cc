/**
 * File: load-large-file-non-blocking-descriptors.cc
 * -------------------------------------------------
 * Algorithmically similar to load-large-file-blocking-descriptors,
 * except that we won't allow read to block if bytes aren't
 * immediately ready.
 */

#include <fcntl.h>    // for open
#include <string>     // for C++ string class
#include <iostream>   // for cout, cerr, endl
#include <cassert>    // for assert macro
#include <unistd.h>   // for close
#include "non-blocking-utils.h"
using namespace std;

static const string kLargeFile = "/afs/ir.stanford.edu/class/cs110/WWW/spring-2018/blob.bin";
int main(int argc, char *argv[]) {
  int fd = open(kLargeFile.c_str(), O_RDONLY | O_NONBLOCK);
  if (fd == -1) {
    cerr << "Could not open file named \"" << kLargeFile << "\"." << endl;
    cerr << "Aborting..." << endl;
    return 1;
  }

  assert(isNonBlocking(fd));

  int numReads = 0;
  int numByteProducingReads = 0;
  size_t numBytes = 0;
  char buffer[1 << 16];
  while (true) {
    ssize_t count = read(fd, buffer, sizeof(buffer));
    numReads++;
    if (count == -1) {
      assert(errno == EWOULDBLOCK || errno == EAGAIN);
      continue;
    }
    if (count == 0) break; // we are truly done
    numByteProducingReads++;
    numBytes += count;
  }

  close(fd);  
  cout << "File size: " << numBytes << " bytes." << endl;
  cout << "Num reads needed: " << numReads << endl;
  cout << "Num reads surfacing one or more bytes: " << numByteProducingReads << endl;
  return 0;
}
