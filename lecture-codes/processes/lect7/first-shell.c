/**
 * File: first-shell.c
 * ----------------
 * A program containing an implementation of a working shell that can
 * execute entered text commands.  It relies on our own implementation
 * of system(), called mysystem(), that creates a process to execute 
 * a given shell command.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include "string.h"
#include "exit-utils.h"

// The max-length shell command the user can enter
static const size_t kMaxLineLength = 2048;

// The exit status for the child if the exec fails (uncomment when implemented)
// static const int kExecFailed = 1;

/* Function: mysystem
 * ------------------
 * Parameters:
 *     command - the shell command to execute in another process
 * Returns: the exit status of the command execution
 *
 * This function executes the given shell command using /bin/sh.
 * It executes it in another process, and waits for that process
 * to complete before returning.  This function returns its exit status.
 */
// Uncomment when implemented
/* static */ int mysystem(char *command) {
    // TODO
    return 0;
}

int main(int argc, char *argv[]) {
  char command[kMaxLineLength];
  while (true) {
    printf("> ");
    fgets(command, sizeof(command), stdin);
    
    // If the user entered Ctl-d, stop
    if (feof(stdin)) {
        break;
    }
    
    // Remove the \n that fgets puts at the end
    command[strlen(command) - 1] = '\0';

    // TODO: use mysystem instead of system
    int commandReturnCode = system(command);
    printf("return code = %d\n", commandReturnCode);
  }
  
  printf("\n");
  return 0;
}
