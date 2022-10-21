/**
 * File: dining-philosophers-with-cv-wait-lambda.cc
 * ---------------------------------------------
 * This program implements the classic dining philosophers
 * simulation.  This is the same as with-cv-wait except it
 * uses the lambda form of cv wait instead of the normal version.
 */

#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include "ostreamlock.h"
#include "random-generator.h"
#include "thread-utils.h"
using namespace std;

/**
 * Defines a collection of timing constants used for 
 * the dining philosopher simulation.
 */

static const unsigned int kLowThinkTime = 100;
static const unsigned int kHighThinkTime = 2000;
static const unsigned int kLowSleepTime = 25;
static const unsigned int kHighSleepTime = 50;

/**
 * Defines the single RandomGenerator class and helper functions used to 
 * generate random timing amounts to allow for variety in the dining 
 * philosopher simulation.
 */

static mutex rgenLock;
static RandomGenerator rgen;

static unsigned int getThinkTime() {
  rgenLock.lock();
  unsigned int value = rgen.getNextInt(kLowThinkTime, kHighThinkTime);
  rgenLock.unlock();
  return value;
}

static unsigned int getEatTime() {
  rgenLock.lock();
  unsigned int value = rgen.getNextInt(kLowSleepTime, kHighSleepTime);
  rgenLock.unlock();
  return value;
}

/**
 * Defines the collection of constants (not related at all to timing)
 * needed for the dining philosophers simulation.
 */

static const size_t kNumPhilosophers = 5;
static const size_t kNumForks = kNumPhilosophers;
static const size_t kNumMeals = 3;

static void waitForPermission(size_t& permits, condition_variable_any& permitsCV, mutex& permitsLock) {
  permitsLock.lock();
  permitsCV.wait(permitsLock, [&permits] { return permits > 0; });
  permits--;
  permitsLock.unlock();
}

static void grantPermission(size_t& permits, condition_variable_any& permitsCV, mutex& permitsLock) {
  permitsLock.lock();
  permits++;
  if (permits == 1) permitsCV.notify_all();
  permitsLock.unlock();
}

static void think(size_t id) {
  cout << oslock << id << " starts thinking." << endl << osunlock;
  sleep_for(getThinkTime());
  cout << oslock << id << " all done thinking. " << endl << osunlock;
}

static void eat(size_t id, mutex& left, mutex& right, size_t& permits, 
  condition_variable_any& permitsCV, mutex& permitsLock) {

  waitForPermission(permits, permitsCV, permitsLock);
  left.lock();
  right.lock();
  cout << oslock << id << " starts eating om nom nom nom." << endl << osunlock;
  sleep_for(getEatTime());
  cout << oslock << id << " all done eating." << endl << osunlock;
  grantPermission(permits, permitsCV, permitsLock);
  left.unlock();
  right.unlock();
}

static void philosopher(size_t id, mutex& left, mutex& right, size_t& permits, 
  condition_variable_any& permitsCV, mutex& permitsLock) {

  for (size_t i = 0; i < kNumMeals; i++) {
    think(id);
    eat(id, left, right, permits, permitsCV, permitsLock);
  }
}

int main(int argc, const char *argv[]) {
  mutex forks[kNumForks];

  // A philosopher needs a permit in order to try to eat
  size_t permits = kNumForks - 1;
  mutex permitsLock;
  condition_variable_any permitsCV;

  thread philosophers[kNumPhilosophers];
  for (size_t i = 0; i < kNumPhilosophers; i++) {
    mutex& left = forks[i];
    mutex& right = forks[(i + 1) % kNumForks];
    philosophers[i] = thread(philosopher, i, ref(left), ref(right), 
      ref(permits), ref(permitsCV), ref(permitsLock));
  }
  for (thread& p: philosophers) p.join();
  return 0;
}
