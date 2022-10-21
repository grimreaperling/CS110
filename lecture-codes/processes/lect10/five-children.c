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
#include "sleep-utils.h" // for snooze

static const size_t kNumChildren = 5;

int main(int argc, char *argv[]) {
  printf("Let my five children play while I take a nap.\n");

  for (size_t kid = 1; kid <= kNumChildren; kid++) {
    if (fork() == 0) {
      sleep(3 * kid); // sleep emulates "play" time
      printf("Child #%zu tired... returns to parent.\n", kid);
      return 0;
    }
  }

  while (1 /* TODO */) {
    printf("At least one child still playing, so parent nods off.\n");
    snooze(5); // custom fn to sleep uninterrupted
    printf("Parent wakes up! ");
  }  
  printf("All children accounted for.  Good job, parent!\n");
  return 0;
}