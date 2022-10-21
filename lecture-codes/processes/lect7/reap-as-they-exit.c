/**
 * File: reap-as-they-exit.c
 * -------------------------
 * The following program spawns off 8 children,
 * each of which returns a different exit status.
 */

#include <unistd.h>     // for fork
#include <stdio.h>      // for printf
#include <sys/wait.h>   // for waitpid
#include <errno.h>      // for errno, ECHILD
#include "exit-utils.h" 

static const int kNumChildren = 8;
static const int kForkFail = 1;
static const int kWaitFail = 2;

int main(int argc, char *argv[]) {
    // Spawn children that immediately exit
    for (size_t i = 0; i < kNumChildren; i++) {
        pid_t pidOrZero = fork();
        exitIf(pidOrZero == -1, kForkFail, stderr, "Fork function failed.\n");
        if (pidOrZero == 0) exit(110 + i);
    }

    // Wait on children in no particular order
    while (true) {
        // Wait for one of our children
        int status;
        pid_t pid = waitpid(-1, &status, 0);
        if (pid == -1) {
            /* If waitpid returns -1, the global variable errno 
             * should be ECHILD, meaning we have no more children
             * to wait on.  If it's not, there was an error.
             */
            exitIf(errno != ECHILD, kWaitFail, stderr, "waitpid failed.\n");
            break;
        }

        // Print out the child's exit information
        if (WIFEXITED(status)) {
            printf("Child %d exited: status %d\n", pid, WEXITSTATUS(status));
        } else {
            printf("Child %d exited abnormally.\n", pid);
        }
    }

    return 0;
}
