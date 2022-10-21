/**
 * File: five-nsync-children.cc
 * ----------------------------
 * This program illustrates how to handle
 * all signals synchronously so as to avoid
 * the unpredictability and instability that
 * comes with signal handlers.
 */

#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>
#include <iostream>
#include <cassert>
#include <vector>
using namespace std;

static const size_t kNumChildren = 5;
static size_t reapChildProcesses(size_t numDone) {
  while (true) {
    pid_t pid = waitpid(-1, NULL, WNOHANG);
    if (pid <= 0) break;
    numDone++;
  }
  return numDone;
}

static void setAlarm(double duration) { // fire SIGALRM 'duration' seconds from now
  int seconds = int(duration);
  int microseconds = 1000000 * (duration - seconds);
  struct itimerval next = {{0, 0}, {seconds, microseconds}};
  setitimer(ITIMER_REAL, &next, NULL);
}

static const double kSleepTime = 5.0;
static void letDadSleep() {
  cout << "At least one child still playing, so dad nods off." << endl;  
  setAlarm(kSleepTime);
}

static void wakeUpDad(size_t numDone) {
  cout << "Dad wakes up and sees " << numDone
       << " " << (numDone == 1 ? "child" : "children") << "." << endl;
  if (numDone < kNumChildren) letDadSleep();
}

static void constructMonitoredSet(sigset_t& monitored, const vector<int>& signals) {
  sigemptyset(&monitored);
  for (int signal: signals) sigaddset(&monitored, signal);
}

static void blockMonitoredSet(const sigset_t& monitored) {
  sigprocmask(SIG_BLOCK, &monitored, NULL);
}

static void unblockMonitoredSet(const sigset_t& monitored) {
  sigprocmask(SIG_UNBLOCK, &monitored, NULL);
}

int main(int argc, char *argv[]) {
  cout << "Let my five children play while I take a nap." << endl;
  sigset_t monitored;
  signal(SIGCHLD, [](int){});
  constructMonitoredSet(monitored, {SIGCHLD, SIGALRM});
  blockMonitoredSet(monitored);
  for (size_t kid = 1; kid <= kNumChildren; kid++) {
    pid_t pid = fork();
    if (pid == 0) {
      unblockMonitoredSet(monitored);
      sleep(3 * kid); // sleep emulates "play" time
      cout << "Child " << kid << " tires... returns to dad." << endl;
      return 0;
    }
  }
  
  size_t numDone = 0;
  bool dadSeesEveryone = false;
  letDadSleep();
  while (!dadSeesEveryone) {
    int delivered;
    sigwait(&monitored, &delivered);
    switch (delivered) {
    case SIGCHLD:
      numDone = reapChildProcesses(numDone);
      break;
    case SIGALRM:
      wakeUpDad(numDone);
      dadSeesEveryone = numDone == kNumChildren;
      break;
    }
  }
  
  cout << "All children accounted for.  Good job, dad!" << endl;
  return 0;
}
