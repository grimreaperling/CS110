/**
 * File: outbound-file-test.cc
 * ---------------------------
 * Demonstrates how one should use the OutboundFile class
 * and can be used to confirm that it works properly.
 */

#include "outbound-file.h"
#include <unistd.h>

int main(int argc, char *argv[]) {
  OutboundFile obf;
  obf.initialize("outbound-file-test.cc", STDOUT_FILENO);
  while (obf.sendMoreData()) {;}
  return 0;
}
