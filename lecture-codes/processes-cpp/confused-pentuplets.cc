/**
 * File: confused-pentuplats.cc
 * ----------------------------
 * This program illustrates why a SIGCHLD handler needs
 * to be implemented on the premise that one of more
 * SIGCHLD signals have come in.
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
      sleep(3);
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
