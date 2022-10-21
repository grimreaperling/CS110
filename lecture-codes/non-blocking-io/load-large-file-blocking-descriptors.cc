/**
 * File: load-large-file-blocking-descriptors.cc
 * ---------------------------------------------
 * This program uses low-level (but otherwise traditional)
 * I/O to read in and count all of the bytes in a very large
 * file.
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
  int fd = open(kLargeFile.c_str(), O_RDONLY);
  if (fd == -1) {
    cerr << "Could not open file named \"" << kLargeFile << "\"." << endl;
    cerr << "Aborting..." << endl;
    return 1;
  }

  assert(isBlocking(fd));

  int numReads = 0;
  size_t numBytes = 0;
  char buffer[1 << 16];
  while (true) {
    ssize_t count = read(fd, buffer, sizeof(buffer));
    assert(count != -1); // simple sanity check, would be more robust in practice
    numReads++;
    if (count == 0) break; // we are truly done
    numBytes += count;
  }

  close(fd);  
  cout << "File size: " << numBytes << " bytes." << endl;
  cout << "Num reads needed: " << numReads << endl;
  return 0;
}
