/**
 * File: five-children.cc
 * ----------------------
 * This program illustrates how a SIGCHLD handler can
 * be used to reap background child processes (and have
 * it work when all background processes take varying lengths
 * of time to complete)
 */

#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include "sleep-utils.h"
using namespace std;

static const size_t kNumChildren = 5;
static size_t numDone = 0;
static void reapChildProcesses(int unused) {
  waitpid(-1, NULL, 0);
  numDone++;
}

int main(int argc, char *argv[]) {
  cout << "Let my five children play while I take a nap." << endl;
  signal(SIGCHLD, reapChildProcesses);
  for (size_t kid = 1; kid <= kNumChildren; kid++) {
    pid_t pid = fork();
    if (pid == 0) {
      sleep(3 * kid); // sleep emulates "play" time
      cout << "Child " << kid << " tires... returns to dad." << endl;
      return 0;
    }
  }
  
  while (numDone < kNumChildren) {
    cout << "At least one child still playing, so dad nods off." << endl;
    snooze(5); // signal-safe version of sleep in sleep-utils.h
    cout << "Dad wakes up! ";
  }
  
  cout << "All children accounted for.  Good job, dad!" << endl;
  return 0;
}
