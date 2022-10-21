/**
 * File: five-children.c
 * ---------------------
 * This program illustrates how a SIGCHLD handler can
 * be used to reap background child processes (and have
 * it work when all background processes take varying lengths
 * of time to complete)
 */

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include "exit-utils.h"  // exitIf, exitUnless
#include "sleep-utils.h" // for snooze

static const int kSignalFailed = 1;
static const int kForkFailed = 2;

static const size_t kNumChildren = 5;
static size_t numChildrenDonePlaying = 0;

static void reapChildProcesses(int sig) {
  while (true) {
    pid_t pid = waitpid(-1, NULL, WNOHANG);
    if (pid <= 0) break;
    numChildrenDonePlaying++;
  } 
}

int main(int argc, char *argv[]) {
  printf("Let my five children play while I take a nap.\n");

  exitIf(signal(SIGCHLD, reapChildProcesses) == SIG_ERR, kSignalFailed,
	 stderr, "Failed to install SIGCHLD handler.\n");

  for (size_t kid = 1; kid <= 5; kid++) {
    pid_t pid = fork();
    exitIf(pid == -1, kForkFailed, stderr, "Child #%zu doesn't want to play.\n", kid);
    if (pid == 0) {
      sleep(3 * kid); // sleep emulates "play" time
      printf("Child #%zu tired... returns to parent.\n", kid);
      return 0;
    }
  }

  while (numChildrenDonePlaying < kNumChildren) {
    printf("At least one child still playing, so parent nods off.\n");
    snooze(5); // implementation in /usr/class/cs110/local/include/sleep-utils.h
    printf("Parent wakes up!");
  }  
  printf("All children accounted for.  Good job, parent!\n");
  return 0;
}
