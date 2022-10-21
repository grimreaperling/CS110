/**
 * File: pipeline.c
 * ----------------
 * Presents the implementation of the pipeline routine, which is similar to subprocess
 * and allows the parent process to spawn two child processes, the first of which has its
 * stdandard output forwarded to the second child's standard input.  The parent then
 * waits for both children to finish.
 *
 * Example test harness runs:
 * 
 * Count the number of lines/words/chars in pipeline-soln.c
 *    ./pipeline-soln cat pipeline-soln.c "|" wc
 * 
 * Sleep for 2 seconds (test that it runs commands in parallel)
 *    ./pipeline-soln sleep 2 "|" sleep 2
 * 
 * Sleep for 1 second, print 0s
 *    ./pipeline-soln sleep 1 "|" wc
 * 
 * Sleep for two seconds, print nothing
 *    ./pipeline-soln echo 'this should not print to the console' "|" sleep 2
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include <error.h>

/* 
 * This function takes in two commands (represented as argv arrays, and spawns
 * two child processes (one for each command) where the STDOUT of the first command
 * is fed into the STDIN of the second command.  The PIDs of the child process
 * are written to the pids array.  This function does not wait for the children
 * to finish before returning.
 */
void pipeline(char *argv1[], char *argv2[], pid_t pids[]) {
  // TODO

  /*
  // This is the code from subprocess
  subprocess_t subprocess(char *command) {
    // Make a pipe
    int fds[2];
    pipe(fds);
    
    pid_t pidOrZero = fork();
    if (pidOrZero == 0) {
        // We are not writing to the pipe, only reading from it
        close(fds[1]);

        // Duplicate the read end of the pipe into STDIN
        dup2(fds[0], STDIN_FILENO);
        close(fds[0]);

        // Run the command
        char *arguments[] = {"/bin/sh", "-c", command, NULL};
        execvp(arguments[0], arguments);
        exitIf(true, kExecFailed, stderr, "execvp failed to invoke this: %s.\n", command);
    }

    close(fds[0]);
    return (subprocess_t) { pidOrZero, fds[1] };
  }
  */
}


/*** TEST HARNESS CODE ***/

static void printArgumentVector(char *argv[]) {
  if (argv == NULL || *argv == NULL) {
    printf("<empty>");
    return;
  }
  
  while (true) {
    printf("%s", *argv);
    argv++;
    if (*argv == NULL) return;
    printf(" ");
  }
}

static void summarizePipeline(char *argv1[], char *argv2[]) {
  printf("Pipeline: ");
  printArgumentVector(argv1);
  printf(" -> ");
  printArgumentVector(argv2);
  printf("\n");
}

static void launchPipedExecutables(char *argv1[], char *argv2[]) {
  summarizePipeline(argv1, argv2);
  pid_t pids[2];
  pipeline(argv1, argv2, pids);
  // pipeline2(argv1, argv2, pids);  
  waitpid(pids[0], NULL, 0);
  waitpid(pids[1], NULL, 0);
}

int main(int argc, char *argv[]) {
  int dividerIndex = -1;
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "|")) {
      dividerIndex = i;
      argv[i] = NULL;
      break;
    }
  }

  if (dividerIndex == -1 || dividerIndex == 1 || dividerIndex == argc - 1) {
    error(1, 0, "Invalid arguments; must specify 2 args arrays with | in between");
  }

  launchPipedExecutables(argv + 1, argv + dividerIndex + 1);

  return 0;
}
