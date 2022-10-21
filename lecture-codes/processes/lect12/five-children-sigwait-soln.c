/**
 * File: five-children-sigwait.c
 * ---------------------
 * This program illustrates how we can wait for SIGCHLD signals
 * using sigwait instead of a signal handler to clean up
 * background child processes.
 */

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/time.h>
#include "exit-utils.h"  // exitIf, exitUnless

static const int kForkFailed = 2;
static const size_t kNumChildren = 5;
static const double kSleepTime = 5;


// This function schedules a SIGALRM signal to be sent to us in 'duration' seconds.
static void setAlarm(double duration) {
  int seconds = (int)duration;
  int microseconds = 1000000 * (duration - seconds);
  struct itimerval next = {{0, 0}, {seconds, microseconds}};
  setitimer(ITIMER_REAL, &next, NULL);
}

/* This function cleans up any terminated children and returns the number cleaned up.
 * This function does not block - it only cleans up children that are terminated now.
 */
static size_t reapChildProcesses() {
  size_t numChildrenReaped = 0;

  while (true) {
    pid_t pid = waitpid(-1, NULL, WNOHANG);
    if (pid <= 0) break;
    numChildrenReaped++;
  } 

  return numChildrenReaped;
}

/* This function waits for signals to come in from the child processes telling
 * us that they have finished.  In the meantime, we nap for kSleepTime second
 * increments, after each increment checking how many children have finished.
 */
static void sleepUntilChildrenDonePlaying(sigset_t monitoredSignals) {
  // The number of children that have been cleaned up
  size_t numChildrenDonePlaying = 0;

  // The number of children that have been cleaned up and seen by parent
  size_t numChildrenParentSees = 0;
  
  printf("At least one child still playing, so parent nods off.\n");
  setAlarm(kSleepTime);

  while (numChildrenParentSees < kNumChildren) {
    // block until we receive a signal
    int delivered;
    sigwait(&monitoredSignals, &delivered);

    if (delivered == SIGCHLD) {
      // If we recieved a SIGCHLD, clean up any finished child processes
      numChildrenDonePlaying += reapChildProcesses();
    } else if (delivered == SIGALRM) {
      // If we received a SIGALRM, parent wakes up and sees children who have finished
      numChildrenParentSees = numChildrenDonePlaying;
      printf("Parent wakes up and sees %zu child(ren).\n", numChildrenParentSees);
      if (numChildrenParentSees < kNumChildren) {
        // Go back to sleep
        printf("At least one child still playing, so parent nods off.\n");
        setAlarm(kSleepTime);
      }
    }
  }
}

/* This function spawns numChildren child processes, each of which first
 * unblocks the specified signals, and then sleeps for some amount of time before
 * terminating.
 */
static void spawnChildren(size_t numChildren, sigset_t signalsToUnblock) {
  for (size_t kid = 1; kid <= numChildren; kid++) {
    pid_t pid = fork();
    exitIf(pid == -1, kForkFailed, stderr, "Child #%zu doesn't want to play.\n", kid);
    if (pid == 0) {
      sigprocmask(SIG_UNBLOCK, &signalsToUnblock, NULL);
      sleep(3 * kid); // sleep emulates "play" time
      printf("Child #%zu tired... returns to parent.\n", kid);
      exit(0);
    }
  }
}

int main(int argc, char *argv[]) {
  printf("Let my five children play while I take a nap.\n");

  // Create a set of relevant signals
  sigset_t monitoredSignals;
  sigemptyset(&monitoredSignals);
  sigaddset(&monitoredSignals, SIGCHLD); 
  sigaddset(&monitoredSignals, SIGALRM);

  // Turn on do not disturb
  sigprocmask(SIG_BLOCK, &monitoredSignals, NULL);

  spawnChildren(kNumChildren, monitoredSignals);
  sleepUntilChildrenDonePlaying(monitoredSignals);

  printf("All children accounted for.  Good job, parent!\n");
  return 0;
}
