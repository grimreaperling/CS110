/**
 * File: copy.c
 * --------------
 * This program is the starter code for an example that shows how we can
 * make a copy of a specified file into another specified file, similar to the
 * `cp` unix command.
 */

#include <stdbool.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <error.h>

// The permissions to set if the destination file doesn't exist
static const int kDefaultPermissions = 0644; // number equivalent of "rw-r--r--"

// The number of bytes we copy at once from source to destination
static const int kCopyIncrement = 1024;


int main(int argc, char *argv[]) {
  // TODO: our code here!
  return 0;
}
