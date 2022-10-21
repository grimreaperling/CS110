/**
 * File: pentuplets.cc
 * -------------------
 * This program correctly implements the SIGCHLD handler
 * to accommodate one or more pending SIGCHLD signals.
 * This program works whether each child sleeps for 3 seconds
 * or each child sleeps for a variable number of seconds.
 */

#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <cassert>
#include "sleep-utils.h"
using namespace std;

static const size_t kNumChildren = 5;
static size_t numDone = 0;
static void reapChildProcesses(int unused) {
  while (true) {
    pid_t pid = waitpid(-1, NULL, WNOHANG); // check out the WNOHANG!!!
    if (pid <= 0) {  // note the < is now a <=
      assert(pid == 0 || errno == ECHILD); // pid could be 0 because of WNOHANG
      break;
    }
    numDone++;
  }
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
