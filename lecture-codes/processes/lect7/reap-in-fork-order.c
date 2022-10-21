/**
 * File: reap-in-fork-order.c
 * --------------------------
 * The following program spawns off 8 children,
 * each of which returns a different exit status.
 * The difference here is that we wait on them
 * in the order in which they are forked.
 */

#include <unistd.h>     // for fork
#include <stdio.h>      // for printf
#include <sys/wait.h>   // for waitpid
#include <errno.h>      // for errno, ECHILD
#include "exit-utils.h" 

static const int kNumChildren = 8;
static const int kForkFail = 1;
static const int kWaitFail = 2;
static const int kExitFail = 4;

int main(int argc, char *argv[]) {
    // We must store all child PIDs to wait on them later
    pid_t children[kNumChildren];

    // Spawn kNumChildren children that immediately exit
    for (size_t i = 0; i < kNumChildren; i++) {
        children[i] = fork();
        exitIf(children[i] == -1, kForkFail, stderr, "Fork function failed.\n");
        if (children[i] == 0) return 110 + i;
    }

    // Wait for the children in the order they were spawned
    for (size_t i = 0; i < kNumChildren; i++) {
        int status;
        pid_t pid = waitpid(children[i], &status, 0);
        
        exitIf(pid != children[i], kWaitFail, stderr,
                   "Intentional wait on child %d failed.\n", children[i]);
        
        exitIf(!WIFEXITED(status) || (WEXITSTATUS(status) != (110 + i)),
               kExitFail, stderr, "Correct child %d exited abnormally.\n");
        printf("Child with pid %d accounted for (return status of %d).\n", children[i], WEXITSTATUS(status));
    }

    return 0;
}
