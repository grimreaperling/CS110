/**
 * File: touch.c
 * --------------
 * This program shows an example of creating a new file with a given
 * name.  Similar to the `touch` unix command, it creates the file if it
 * doesn't exist. But in this case, if it does exist, it throws an error.
 */

#include <fcntl.h>    // for open
#include <unistd.h>   // for read, write, close
#include <stdio.h>
#include <errno.h>

static const int kDefaultPermissions = 0644; // number equivalent of "rw-r--r--"

int main(int argc, char *argv[]) {
  // Create the file
  int fd = open(argv[1], O_WRONLY | O_CREAT | O_EXCL, kDefaultPermissions);

  // If an error occured, print out more information
  if (fd == -1) {
    printf("There was a problem creating \"%s\"!\n", argv[1]);

    if (errno == EEXIST) {
      printf("The file already exists.\n");
    } else {
      printf("Unknown errno: %d\n", errno);
    }
    return -1;
  }

  // Close the file now that we are done with it
  close(fd);
  return 0;
}