/**
 * File: copy-extended.c
 * --------------
 * This program is an implementation of an extended copy command that shows how we can
 * make a copy of a specified file into multiple other specified files, as well
 * as printing the contents to the terminal.  Right now it's the solution from the
 * copy.c program; our task is to build on it!
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


/* This function copies the contents of the source file into the destination
 * file in chunks until the entire file is copied.
 */
void copyContents(int sourceFD, int destinationFD) {
  char buffer[kCopyIncrement];
  while (true) {
    // Read the next chunk of bytes from the source
    ssize_t bytesRead = read(sourceFD, buffer, sizeof(buffer));
    if (bytesRead == 0) break;

    // Write that chunk to the destination
    size_t bytesWritten = 0;
    while (bytesWritten < bytesRead) {
      ssize_t count = write(destinationFD, buffer + bytesWritten, bytesRead - bytesWritten);
      bytesWritten += count;
    }
  }
}


int main(int argc, char *argv[]) {
  // Open the specified source file for reading
  int sourceFD = open(argv[1], O_RDONLY);
  
  // Open the specified destination file for writing, creating if nonexistent
  int destinationFD = open(argv[2], O_WRONLY | O_CREAT | O_EXCL, kDefaultPermissions);

  copyContents(sourceFD, destinationFD);

  // Close the files
  close(sourceFD);
  close(destinationFD);

  return 0;
}
