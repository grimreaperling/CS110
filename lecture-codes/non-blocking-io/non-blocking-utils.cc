/**
 * File: non-blocking-utils.cc
 * ---------------------------
 * Presents the implementation of setAsNonBlocking.
 */

#include "non-blocking-utils.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <cassert>

bool isSocketDescriptor(int descriptor) {
  struct stat st;
  fstat(descriptor, &st);
  return S_ISSOCK(st.st_mode);
}

void setAsNonBlocking(int descriptor) {
  int flags = fcntl(descriptor, F_GETFL);
  if (flags == -1) flags = 0; // if first call to fcntl fails, just go with 0
  fcntl(descriptor, F_SETFL, flags | O_NONBLOCK); // preserve other set flags
}

void setAsBlocking(int descriptor) {
  int flags = fcntl(descriptor, F_GETFL);
  if (flags == -1) flags = 0; // if first call to fcntl fails, just go with 0
  fcntl(descriptor, F_SETFL, flags & ~O_NONBLOCK); // suppress blocking bit, preserve all others
}

bool isNonBlocking(int descriptor) {
  return !isBlocking(descriptor);
}

bool isBlocking(int descriptor) {
  return (fcntl(descriptor, F_GETFL) & O_NONBLOCK) == 0;
}
