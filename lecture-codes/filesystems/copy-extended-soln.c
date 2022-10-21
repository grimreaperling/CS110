/**
 * File: copy.c
 * --------------
 * This program is an implementation of an extended copy command that shows how we can
 * make a copy of a specified file into multiple other specified files, as well
 * as printing the contents to the terminal.
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


/* This function copies the contents of the source file into each destination
 * file in chunks until the entire file is copied.
 */
void copyContents(int sourceFD, int destinationFDs[], int numDestinationFDs) {
  char buffer[kCopyIncrement];
  while (true) {
    // Read the next chunk of bytes from the source
    ssize_t bytesRead = read(sourceFD, buffer, sizeof(buffer));
    if (bytesRead == 0) break;

    // Write that chunk to every destination
    for (size_t i = 0; i < numDestinationFDs; i++) {
      size_t bytesWritten = 0;
      while (bytesWritten < bytesRead) {
        ssize_t count = write(destinationFDs[i], buffer + bytesWritten, bytesRead - bytesWritten);
        bytesWritten += count;
      }
    }
  }
}

int main(int argc, char *argv[]) {
  // Open the specified source file for reading
  int sourceFD = open(argv[1], O_RDONLY);
  
  // Open the specified destination file(s) for writing, creating if nonexistent
  int destinationFDs[argc - 1];

  // Include the terminal (STDOUT) as the first "file" so it's also printed
  destinationFDs[0] = STDOUT_FILENO;

  for (size_t i = 2; i < argc; i++) {
    destinationFDs[i - 1] = open(argv[i], O_WRONLY | O_CREAT | O_EXCL, kDefaultPermissions);
  }

  copyContents(sourceFD, destinationFDs, argc - 1);

  // Close the files (except for STDOUT)
  close(sourceFD);
  for (size_t i = 1; i < argc - 1; i++) {
    close(destinationFDs[i]);
  }

  return 0;
}
