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

// Constants for different error codes we may return
static const int kWrongArgumentCount = 1;
static const int kSourceFileNonExistent = 2;
static const int kDestinationFileOpenFailure = 4;
static const int kReadFailure = 8;
static const int kWriteFailure = 16;

// The permissions to set if the destination file doesn't exist
static const int kDefaultPermissions = 0644; // number equivalent of "rw-r--r--"

// The number of bytes we copy at once from source to destination
static const int kCopyIncrement = 1024;


/* This function copies the contents of the source file into each destination
 * file in chunks until the entire file is copied.  If an error occurred
 * while reading or writing, this function throws an error.
 */
void copyContents(int sourceFD, int destinationFDs[], int numDestinationFDs) {
  char buffer[kCopyIncrement];
  while (true) {
    // Read the next chunk of bytes from the source
    ssize_t bytesRead = read(sourceFD, buffer, sizeof(buffer));
    if (bytesRead == 0) break;
    if (bytesRead == -1) {
      error(kReadFailure, 0, "lost access to file while reading.");
    }

    // Write that chunk to every destination
    for (size_t i = 0; i < numDestinationFDs; i++) {
      size_t bytesWritten = 0;
      while (bytesWritten < bytesRead) {
        ssize_t count = write(destinationFDs[i], buffer + bytesWritten, bytesRead - bytesWritten);
        if (count == -1) {
          error(kWriteFailure, 0, "lost access to file while writing.");
        }
        bytesWritten += count;
      }
    }
  }
}

int main(int argc, char *argv[]) {
  // Ensure the user specified 3 arguments
  if (argc < 3) {
    error(kWrongArgumentCount, 0, "%s <source-file> <destination-file1> <destination-file2> ... .", argv[0]);
  }
  
  // Open the specified source file for reading
  int sourceFD = open(argv[1], O_RDONLY);
  if (sourceFD == -1) {
    error(kSourceFileNonExistent, 0, "%s: source file could not be opened.", argv[1]);
  }
  
  // Open the specified destination file(s) for writing, creating if nonexistent
  int destinationFDs[argc - 1];

  // Include the terminal (STDOUT) as the first "file" so it's also printed
  destinationFDs[0] = STDOUT_FILENO;

  for (size_t i = 2; i < argc; i++) {
    destinationFDs[i - 1] = open(argv[i], O_WRONLY | O_CREAT | O_EXCL, kDefaultPermissions);
    if (destinationFDs[i - 1] == -1) {
      if (errno == EEXIST) {
        error(kDestinationFileOpenFailure, 0, "%s: destination file already exists.", argv[2]);
      } else {
        error(kDestinationFileOpenFailure, 0, "%s: destination file could not be created.", argv[2]);
      }
    }
  }

  copyContents(sourceFD, destinationFDs, argc - 1);

  // Close the source file
  if (close(sourceFD) == -1) {
    error(0, 0, "%s: had trouble closing file.", argv[1]);
  }

  // Close the destination files
  for (size_t i = 1; i < argc - 1; i++) {
    if (close(destinationFDs[i]) == -1) {
      error(0, 0, "%s: had trouble closing file.", argv[2 + i]);
    }
  }

  return 0;
}
