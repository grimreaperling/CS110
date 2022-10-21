/**
 * File: first-shell-soln-bg.c
 * ----------------
 * A program containing an implementation of a working shell that can
 * execute entered text commands, and execute commands in the background when
 * they end with &.  It relies on our own implementation
 * of system() expanded, called executeCommand, to specify whether to run 
 * something in the background (return immediately) or wait.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include "string.h"
#include "exit-utils.h"

// The max-length shell command the user can enter
static const size_t kMaxLineLength = 2048;

// The exit status for the child if the exec fails
static const int kExecFailed = 1;

/* Function: executeCommand
 * ------------------
 * Parameters:
 *     command - the shell command to execute in another process
 *     inBackground - whether to run the command in the background or block until finished
 *
 * This function executes the given shell command using /bin/sh.
 * It executes it in another process, and waits for that process to complete
 * if run in the foreground, or immediately returns if run in the background.
 */
static void executeCommand(char *command, bool inBackground) {
    pid_t pidOrZero = fork();
    if (pidOrZero == 0) {
        // If we are the child, execute the shell command
        char *arguments[] = {"/bin/sh", "-c", command, NULL};
        execvp(arguments[0], arguments);
        // If the child gets here, there was an error
        exitIf(true, kExecFailed, stderr, "execvp failed to invoke this: %s.\n", command);
    }

    // If we are the parent, either wait or return immediately
    if (inBackground) {
        printf("%d %s\n", pidOrZero, command);
    } else {
        waitpid(pidOrZero, NULL, 0);
    }
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

        if (strcmp(command, "quit") == 0) {
            break;
        }

        bool isbg = command[strlen(command) - 1] == '&';
        if (isbg) {
            command[strlen(command) - 1] = '\0';
        }

        executeCommand(command, isbg);
    }
  
    printf("\n");
    return 0;
}
