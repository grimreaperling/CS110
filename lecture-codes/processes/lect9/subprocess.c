/**
 * File: subprocess.c
 * ------------------
 * Implements the subprocess routine, which is similar to popen and allows
 * the parent process to spawn a child process, print to its standard output,
 * and then suspend until the child process has finished.
 */

#include <unistd.h>
#include <sys/wait.h>
#include "exit-utils.h"
#include <string.h>

// The exit status for the child if the exec fails
static const int kExecFailed = 1;

// A struct that stores a PID and FD we can write to
typedef struct subprocess_t {
    pid_t pid;
    int supplyfd;
} subprocess_t;

/* subprocess runs the given shell command in the background
 * in a child process and returns the PID of that child process
 * as well as a file descriptor we can write to to write to that
 * child's STDIN.
 */
subprocess_t subprocess(char *command) {
    // TODO

    /*
    // This is the code from mysystem
    static int mysystem(char *command) {
        pid_t pidOrZero = fork();
        if (pidOrZero == 0) {
            // If we are the child, execute the shell command
            char *arguments[] = {"/bin/sh", "-c", command, NULL};
            execvp(arguments[0], arguments);
            // If the child gets here, there was an error
            exitIf(true, kExecFailed, stderr, "execvp failed to invoke this: %s.\n", command);
        }

        // If we are the parent, wait for the child
        int status;
        waitpid(pidOrZero, &status, 0);
        return WIFEXITED(status) ? WEXITSTATUS(status) : -WTERMSIG(status);
    }
    */
}

int main(int argc, char *argv[]) {
    // Spawn a child that is running the sort command
    subprocess_t sp = subprocess("/usr/bin/sort");

    // We would like to feed these words as input to sort
    const char *words[] = {
        "felicity", "umbrage", "susurration", "halcyon", 
        "pulchritude", "ablution", "somnolent", "indefatigable"
    };

    // write each word on its own line to the STDIN of the child sort process
    for (size_t i = 0; i < sizeof(words) / sizeof(words[0]); i++) {
        dprintf(sp.supplyfd, "%s\n", words[i]);
    }

    // Close the write FD to indicate the input is closed
    close(sp.supplyfd);

    // Wait for the child to finish before exiting
    int status;
    pid_t pid = waitpid(sp.pid, &status, 0);
    return pid == sp.pid && WIFEXITED(status) ? WEXITSTATUS(status) : -127;
}
