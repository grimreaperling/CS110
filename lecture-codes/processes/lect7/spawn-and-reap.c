/**
 * File: spawn-and-reap.c
 * --------------------------
 * The following program spawns off a child and
 * then immediately waits on it to finish, and repeats
 * this 8 times.  Each child returns a different
 * exit status.
 */

#include <unistd.h>     // for fork
#include <stdio.h>      // for printf
#include <sys/wait.h>   // for waitpid
#include "exit-utils.h" 

static const int kNumChildren = 8;
static const int kForkFail = 1;
static const int kWaitFail = 2;

int main(int argc, char *argv[]) {
    for (size_t i = 0; i < kNumChildren; i++) {
        pid_t pidOrZero = fork();
        exitIf(pidOrZero == -1, kForkFail, stderr, "Fork function failed.\n");
        if (pidOrZero == 0) {
            printf("Hello from child %d!\n", getpid());
            return 110 + i;
        }

        int status;
        pid_t pid = waitpid(pidOrZero, &status, 0);
        exitIf(pid != pidOrZero, kWaitFail, stderr, "wait on child %d failed.\n", pidOrZero);
        
        if (WIFEXITED(status)) {
            printf("Child with pid %d exited normally with status %d\n", pid, WEXITSTATUS(status));
        } else {
            printf("Child with pid %d exited abnormally\n", pid);
        }
    }

    return 0;
}
