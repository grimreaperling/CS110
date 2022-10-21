#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <cassert>
using namespace std;

struct note {
  string pitch;    // e.g. "G#4", "Bb2", etc
  double start;    // time from launch when note should be played (in seconds)
  double duration; // in seconds
};

static void usage() {
  cerr << "Usage: ./guitar [song-file.txt]" << endl;
  exit(0);
}

static void initializeSong(vector<note>& song, const char *filename) {
  if (filename == NULL) filename = "strum.txt";
  ifstream infile(filename);
  if (!infile) cerr << "We couldn't find the file named \"" << filename << "\".  Check the file name" << endl;
  while (true) {
    note n;
    infile >> n.pitch >> n.start >> n.duration;
    if (infile.fail()) break;
    song.push_back(n);
  }
}

static void setAlarm(double duration) { // fire SIGALRM 'secs' seconds from now
  int seconds = int(duration);
  int microseconds = 1000000 * (duration - seconds);
  struct itimerval next = {{0, 0}, {seconds, microseconds}};
  setitimer(ITIMER_REAL, &next, NULL);
}

static void stopPlaying(const set<pid_t>& processes) {
  for (pid_t pid: processes) kill(pid, SIGKILL);
  setAlarm(0); // clears the timer
}

static void reapChildProcesses(set<pid_t>& processes) {
  while (true) {
    pid_t pid = waitpid(-1, NULL, WNOHANG);
    if (pid <= 0) break;
    processes.erase(pid);
  }
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

static size_t playNextNotes(const vector<note>& song, size_t pos,
			    const sigset_t& monitored, set<pid_t>& processes) {
  double current = song[pos].start;
  while (pos < song.size() && song[pos].start == current) {
    pid_t pid = fork();
    if (pid == 0) {
      unblockMonitoredSet(monitored);
      string duration = to_string(song[pos].duration);
      const char *argv[] = {
      	"play", "-q", "-n", "synth", duration.c_str(), "pl",  
        song[pos].pitch.c_str(), "remix", "-", "norm", "-20", NULL
      };
      execvp(argv[0], (char **) argv);
      exit(0);
    }
    pos++;
    processes.insert(pid);
  }
  if (pos < song.size()) setAlarm(song[pos].start - current);
  return pos;
}

static void playSong(const vector<note>& song) {
  size_t pos = 0;
  set<pid_t> processes;
  sigset_t monitored;
  signal(SIGCHLD, [](int){}); // hack so things work on macOS
  constructMonitoredSet(monitored, {SIGINT, SIGALRM, SIGCHLD});
  blockMonitoredSet(monitored);
  raise(SIGALRM);
  while (pos < song.size() || !processes.empty()) {
    int delivered;
    sigwait(&monitored, &delivered);
    switch (delivered) {
    case SIGINT:
      stopPlaying(processes);
      pos = song.size(); // sentinel value stating that no more notes should be played
      break;
    case SIGALRM:
      pos = playNextNotes(song, pos, monitored, processes);
      break;
    case SIGCHLD:
      reapChildProcesses(processes);
      break;
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc > 2) usage();
  vector<note> song;
  initializeSong(song, argv[1]);
  playSong(song);
  return 0;
}
