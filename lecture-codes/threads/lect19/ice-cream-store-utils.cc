/**
 * File: ice-cream-store-utils.cc
 * -------------------------------
 * Defines module state needed to generate random
 * numbers in a multithreaded environment.
 */

#include <mutex>
#include <iostream>
#include "random-generator.h"
#include "ostreamlock.h"
#include "thread-utils.h"
using namespace std;

// The range of ice cream cones that a customer may order
static const size_t kMinConeOrder = 1;
static const size_t kMaxConeOrder = 4;

/* The range of time, in milliseconds, that a customer could
 * browse for before joining clerks.
 */
static const size_t kMinBrowseTimeMS = 100;
static const size_t kMaxBrowseTimeMS = 300;

/* The range of time, in milliseconds, that a clerk could
 * take to make an ice cream cone.
 */
static const size_t kMinPrepTimeMS = 50;
static const size_t kMaxPrepTimeMS = 300;

/* The range of time, in milliseconds, that a manager could
 * take to inspect an ice cream cone.
 */
static const size_t kMinInspectionTimeMS = 20;
static const size_t kMaxInspectionTimeMS = 100;

// The probability that the manager approves a given ice cream cone
static const double kConeApprovalProbability = 0.3;

// We share one random generator, so we need a lock for accessing it.
static mutex rgenLock;
static RandomGenerator rgen;

static size_t getBrowseTimeMS() {
  unique_lock<mutex> rgenUniqueLock(rgenLock);
  return rgen.getNextInt(kMinBrowseTimeMS, kMaxBrowseTimeMS);
}

static size_t getPrepTimeMS() {
  unique_lock<mutex> rgenUniqueLock(rgenLock);
  return rgen.getNextInt(kMinPrepTimeMS, kMaxPrepTimeMS);
}

static size_t getInspectionTimeMS() {
  unique_lock<mutex> rgenUniqueLock(rgenLock);
  return rgen.getNextInt(kMinInspectionTimeMS, kMaxInspectionTimeMS);
}

static bool getInspectionOutcome() {
  unique_lock<mutex> rgenUniqueLock(rgenLock);
  return rgen.getNextBool(kConeApprovalProbability);
}

size_t getNumCones() {
  unique_lock<mutex> rgenUniqueLock(rgenLock);
  return rgen.getNextInt(kMinConeOrder, kMaxConeOrder);
}

void browse() {
  cout << oslock << "Customer starts to kill time." << endl << osunlock;
  size_t browseTimeMS = getBrowseTimeMS();
  sleep_for(browseTimeMS);
  cout << oslock << "Customer just killed " << double(browseTimeMS) / 1000
       << " seconds." << endl << osunlock;
}

void makeCone(size_t coneID, size_t customerID) {
  cout << oslock << "    Clerk starts to make ice cream cone #" << coneID 
       << " for customer #" << customerID << "." << endl << osunlock;
  size_t prepTimeMS = getPrepTimeMS();
  sleep_for(prepTimeMS);
  cout << oslock << "    Clerk just spent " << double(prepTimeMS) / 1000 
       << " seconds making ice cream cone #" << coneID 
       << " for customer #" << customerID << "." << endl << osunlock;
}

bool inspectCone() {
  cout << oslock << "  Manager is presented with an ice cream cone." 
       << endl << osunlock;

  // Sleep for some random amount of time to simulate inspecting the cone
  size_t inspectionTimeMS = getInspectionTimeMS();
  sleep_for(inspectionTimeMS);

  // Generate an inspection decision
  bool passed = getInspectionOutcome();
  string verb = passed ? "APPROVED" : "REJECTED";
  cout << oslock << "  Manager spent " << double(inspectionTimeMS) / 1000
       << " seconds analyzing presented ice cream cone and " << verb << " it." 
       << endl << osunlock;
  return passed;
}
