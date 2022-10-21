/**
 * File: greeters.cc
 * -----------------
 * More sophisticated examples illustrating how one can use the C++ threads to spawn off
 * multiple copies of the same threads.
 */

#include <thread>         // prvides for C++11 threads
#include <iostream>       // provides cout, endl
#include <cstdlib>        // provides srandom, random (which we pretend are thread-safe, even though they aren't)
#include "ostreamlock.h"  // provides custom CS110 iomanipulators for locking down streams
#include "thread-utils.h" // for CS110 sleep_for function
using namespace std;

static void greet(size_t id) {
    for (size_t i = 0; i < id; i++) {
        cout << oslock << "Greeter #" << id << " says 'Hello!'" << endl << osunlock;
        struct timespec ts = {
            0, random() % 1000000000
        };
        nanosleep(&ts, NULL);
    }
    cout << oslock << "Greeter #" << id << " has issued all of his hellos, "
         << "so he goes home!" << endl << osunlock;
}

static const size_t kNumGreeters = 6;
int main(int argc, char *argv[]) {
  srandom(time(NULL));
  cout << "Welcome to Greetland!" << endl;
  thread greeters[kNumGreeters];
  for (size_t i = 0; i < kNumGreeters; i++) 
    greeters[i] = thread(greet, i + 1);
  for (thread& greeter: greeters) 
    greeter.join();
  cout << "Everyone's all greeted out!" << endl;
  return 0;
}
