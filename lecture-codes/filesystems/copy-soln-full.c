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


/* This function copies the contents of the source file into the destination
 * file in chunks until the entire file is copied.  If an error occurred
 * while reading or writing, this function throws an error.
 */
void copyContents(int sourceFD, int destinationFD) {
  char buffer[kCopyIncrement];
  while (true) {
    // Read the next chunk of bytes from the source
    ssize_t bytesRead = read(sourceFD, buffer, sizeof(buffer));
    if (bytesRead == 0) break;
    if (bytesRead == -1) {
      error(kReadFailure, 0, "lost access to file while reading.");
    }

    // Write that chunk to the destination
    size_t bytesWritten = 0;
    while (bytesWritten < bytesRead) {
      ssize_t count = write(destinationFD, buffer + bytesWritten, bytesRead - bytesWritten);
      if (count == -1) {
        error(kWriteFailure, 0, "lost access to file while writing.");
      }
      bytesWritten += count;
    }
  }
}


int main(int argc, char *argv[]) {
  // Ensure the user specified 3 arguments
  if (argc != 3) {
    error(kWrongArgumentCount, 0, "%s <source-file> <destination-file>.", argv[0]);
  }
  
  // Open the specified source file for reading
  int sourceFD = open(argv[1], O_RDONLY);
  if (sourceFD == -1) {
    error(kSourceFileNonExistent, 0, "%s: source file could not be opened.", argv[1]);
  }
  
  // Open the specified destination file for writing, creating if nonexistent
  int destinationFD = open(argv[2], O_WRONLY | O_CREAT | O_EXCL, kDefaultPermissions);
  if (destinationFD == -1) {
    if (errno == EEXIST) {
      error(kDestinationFileOpenFailure, 0, "%s: destination file already exists.", argv[2]);
    } else {
      error(kDestinationFileOpenFailure, 0, "%s: destination file could not be created.", argv[2]);
    }
  }

  copyContents(sourceFD, destinationFD);

  // Close the source file
  if (close(sourceFD) == -1) {
    error(0, 0, "%s: had trouble closing file.", argv[1]);
  }

  // Close the destination file
  if (close(destinationFD) == -1) {
    error(0, 0, "%s: had trouble closing file.", argv[2]);
  }

  return 0;
}
