/* Program: friends.cc
 * -------------------
 * This program spawns threads to each greet you, and we wait for all
 * threads to finish before terminating.
 */

#include <iostream>       // for cout, endl;
#include <thread>         // for C++11 thread support
#include "ostreamlock.h"  // for CS110 iomanipulators (oslock, osunlock) used to lock down streams
using namespace std;


static const size_t kNumFriends = 6;

static void greeting(size_t i) {
    cout << oslock << "Hello, world! I am thread " << i << endl << osunlock;
}

int main(int argc, char *argv[]) {
  cout << "Let's hear from " << kNumFriends << " threads." << endl;  

   // declare array of empty thread handles
  thread friends[kNumFriends];

  // Spawn threads
  for (size_t i = 0; i < kNumFriends; i++) {
      friends[i] = thread(greeting, i); 
  }

  // Wait for threads
  for (size_t i = 0; i < kNumFriends; i++) {
     friends[i].join();    
  }

  cout << "Everyone's said hello!" << endl;
  return 0;
}

