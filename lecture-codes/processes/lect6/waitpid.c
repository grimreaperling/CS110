/**
 * File: waitpid.c
 * ----------------
 * Here's is a program where a parent waits for its child.
 */

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include "exit-utils.h"

static const int kForkFailed = 1;
static const int kWaitFailed = 2;

int main(int argc, char *argv[]) {
    printf("Before.\n");
    pid_t pidOrZero = fork();
    exitIf(pidOrZero == -1, kForkFailed, stderr, "Fork function failed.\n");
    
    if (pidOrZero == 0) {
        sleep(2);
        printf("I (the child) slept and the parent still waited up for me.\n");
    } else {
        pid_t result = waitpid(pidOrZero, NULL, 0);
        exitIf(result != pidOrZero, kWaitFailed,
                   stderr, "Parent's wait for child process with pid %d failed.\n", pidOrZero);
        printf("I (the parent) finished waiting for the child.  This always prints last.\n");
    }

    return 0;
}
