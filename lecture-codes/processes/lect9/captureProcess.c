/**
 * File: captureProcess.c
 * ------------------
 * Implements the captureProcess routine, which is similar to subprocess and allows
 * the parent process to spawn a child process, read from its standard output,
 * and then suspend until the child process has finished.
 */

#include <unistd.h>
#include <sys/wait.h>
#include "exit-utils.h"
#include <string.h>

// The exit status for the child if the read or exec fails
static const int kExecFailed = 1;
static const int kReadFailed = 8;

// A struct that stores a PID and FD we can read from
typedef struct process_t {
    pid_t pid;
    int readfd;
} process_t;

/* captureProcess runs the given shell command in the background
 * in a child process and returns the PID of that child process
 * as well as a file descriptor we can read from to read that
 * child's STDOUT.
 */
process_t captureProcess(char *command) {
    int fds[2];
    pipe(fds);

    pid_t pidOrZero = fork();
    if (pidOrZero == 0) {
        // We are not reading from the pipe, only writing to it
        close(fds[0]);

        // Duplicate the write end of the pipe into STDOUT
        dup2(fds[1], STDOUT_FILENO);
        close(fds[1]);

        // Run the command
        char *arguments[] = {"/bin/sh", "-c", command, NULL};
        execvp(arguments[0], arguments);
        exitIf(true, kExecFailed, stderr, "execvp failed to invoke this: %s.\n", command);
    }

    close(fds[1]);
    return (process_t) { pidOrZero, fds[0] };
}



int main(int argc, char *argv[]) {
    // Spawn a child that is running the sort command
    process_t sp = captureProcess("/bin/ls");

    while (true) {
        char buffer[1024];
        ssize_t bytesRead = read(sp.readfd, buffer, sizeof(buffer) - 1);
        if (bytesRead == 0) break;
        exitIf(bytesRead == -1, kReadFailed, stderr, "read failed\n");
        buffer[sizeof(buffer) - 1] = '\0';
        printf("[%s]", buffer);
    }

    // Close the read FD because we are done reading
    close(sp.readfd);

    // Wait for the child to finish before exiting
    int status;
    pid_t pid = waitpid(sp.pid, &status, 0);
    return pid == sp.pid && WIFEXITED(status) ? WEXITSTATUS(status) : -127;
}
