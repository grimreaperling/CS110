#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include "exit-utils.h"
#include <string.h>

static const int kPipeFailed = 1;
static const int kForkFailed = 2;
static const int kReadFailure = 8;
static const int kWriteFailure = 16;

static const char * kPipeMessage = "Hello, this message is coming through a pipe.";

// A wrapper around write() that writes all bytes, looping if necessary
ssize_t writeAll(int fd, const void *buf, size_t count) {
    size_t bytesWritten = 0;
    while (bytesWritten < count) {
        ssize_t justWritten = write(fd, (char *)buf + bytesWritten, count - bytesWritten);
        if (count == -1) return -1;
        bytesWritten += justWritten;
    }

    return bytesWritten;
}

int main(int argc, char *argv[]) {
    int fds[2];
    int result = pipe(fds);
    exitIf(result == -1, kPipeFailed, stderr, "pipe function failed.\n");

    size_t bytesSent = strlen(kPipeMessage) + 1;

    pid_t pidOrZero = fork();
    exitIf(pidOrZero == -1, kForkFailed, stderr, "fork function failed.\n");
    if (pidOrZero == 0) {
        // In the child, we only read from the pipe
        close(fds[1]);
        char buffer[bytesSent];
        ssize_t bytesRead = read(fds[0], buffer, sizeof(buffer));
        close(fds[0]);
        exitIf(bytesRead == -1, kReadFailure, stderr, "read function failed.\n");
        printf("Message from parent: %s\n", buffer);
        return 0;
    }

    // In the parent, we only write to the pipe
    close(fds[0]);
    ssize_t count = writeAll(fds[1], kPipeMessage, bytesSent);
    exitIf(count == -1, kWriteFailure, stderr, "write function failed.\n");
    close(fds[1]);
    waitpid(pidOrZero, NULL, 0);
    return 0;
}
