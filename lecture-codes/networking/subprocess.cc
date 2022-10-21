/**
 * File: subprocess.cc
 * -------------------
 * Presents the implementation of the subprocess routine.
 */

#include "subprocess.h"
#include <stdio.h>  // for fprintf, stderr
#include <stdlib.h> // for exit
#include <fcntl.h>
#include "fork-utils.h"
using namespace std;

static void createPipe(int fds[]) {
  if (pipe2(fds, O_CLOEXEC) == -1)
    throw SubprocessException("An attempt to create pipe failed.");
}

static int createProcess() {
  int pid = fork();
  if (pid == -1)
    throw SubprocessException("Failed to create a new process using fork().");
  return pid;
}

static void closeEndpoint(int endpoint) {
  if (endpoint == kNotInUse) return;
  if (close(endpoint) == -1)
    throw SubprocessException("The file descriptor " + to_string(endpoint) + " could not be closed.");
}

static void overwriteDescriptor(int newfd, int oldfd) {
  if (dup2(newfd, oldfd) == -1)
    throw SubprocessException("Failed to donate resources of descriptor " + to_string(newfd) + " to " + to_string(oldfd) + ".");
}

subprocess_t subprocess(char *argv[], bool supplyChildInput, bool ingestChildOutput) {
  int pipes[4] = { kNotInUse, kNotInUse, kNotInUse, kNotInUse };
  if (supplyChildInput) createPipe(pipes);
  if (ingestChildOutput) createPipe(pipes + 2);
  subprocess_t proc = { createProcess(), pipes[1], pipes[2] };
  if (proc.pid == 0) {
    if (supplyChildInput) overwriteDescriptor(pipes[0], STDIN_FILENO);
    if (ingestChildOutput) overwriteDescriptor(pipes[3], STDOUT_FILENO);
    execvp(argv[0], argv); // returning -1 would be a user error we shouldn't identify as an exceptional error
    fprintf(stderr, "Command not found: %s\n", argv[0]); // no error checking to be done
    exit(0);
  }
  
  closeEndpoint(pipes[0]);
  closeEndpoint(pipes[3]);
  return proc;
}
