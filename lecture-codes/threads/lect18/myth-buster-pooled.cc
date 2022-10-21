/**
 * File: myth-buster-pooled.cc
 * -------------------------------
 * A multi-threaded program that makes network calls in parallel to find out
 * how many CS110 student processes are running on each of the myth machines.
 * Same as myth-buster-concurrent but uses a thread pool instead of spawning
 * threads manually.
 */

#include <iostream>
#include <fstream>
#include <unordered_set>
#include <map>
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>
#include "myth-buster-service.h"
#include "ostreamlock.h"
#include "string-utils.h"
#include "thread-pool-release.h"
using namespace std;
using namespace release;

// Filename containing student sunets, 1 per line
static const string kCS110StudentIDsFile = "studentsunets.txt";

// The range of myth machine numbers to poll
static const int kMinMythMachine = 51;
static const int kMaxMythMachine = 66;

// The exit status to use if the student SUNETs file can't be read
static const int kFileInaccessible = 1;

// The maximum number of threads that can be working simultaneously
static const int kMaxNumSimultaneousThreads = 8;


/* Function: readStudentSUNETsFile
 * ------------------------------
 * Parameters:
 *    - sunetIDs: a set that we should populate with sunets from the given file
 *    - filename: the file to read in SUNETs from (assumed 1 SUNET per line)
 *
 * Returns: N/A
 */
static void readStudentSUNETsFile(unordered_set<string>& sunetIDs, 
  const string& filename) {

  // Attempt to open the file
  ifstream infile(filename.c_str());
  if (infile.fail()) {
    cerr << "CS110 Student SUNet ID list not found." << endl;
    exit(kFileInaccessible);
  }
  
  // Read each line and add it to the set
  while (true) {
    string sunetID;
    getline(infile, sunetID);
    if (infile.fail()) return;
    sunetID = trim(sunetID);
    sunetIDs.insert(sunetID);
  }
}

/* Function: printMythMachineWithFewestProcesses
 * ---------------------------------------------
 * Parameters:
 *    - processCountMap: map from myth num to # CS110 processes on that myth
 *
 * Returns: N/A
 *
 * Prints out the entry in the map with the fewest # CS110 processes, or in the
 * case of a tie, the entry in the map with the fewest processes and 
 * highest myth number.
 */
static void printMythMachineWithFewestProcesses(const map<int, int>& processCountMap) {
  /* Make a comparison function to compare the map entries in increasing process
   * count order, or in the case of ties, in decreasing myth number order.
   */
  auto compfn = [](const pair<int, int>& one, const pair<int, int>& two) -> bool {
    return one.second < two.second || (one.second == two.second && one.first > two.first);
  };

  auto leastLoaded = min_element(processCountMap.cbegin(), processCountMap.cend(), compfn);
  cout << "Machine least loaded by CS110 students: myth" << leastLoaded->first << endl;
  cout << "Number of CS110 processes on least loaded machine: " << leastLoaded->second << endl;
}

/* Function: countCS110ProcessesForMyth
 * ------------------------------------
 * Parameters:
 *    - mythNum: the number myth machine to count processes for
 *    - sunetIDs: set of SUNET Ids for whom we should count processes
 *    - processCountMap: a map from myth number to CS110 process count that we
 *                      should fill in for the specified myth machine
 *    - processCountMapLock: a lock around the map to avoid races when updating
 *
 * Returns: N/A
 */
static void countCS110ProcessesForMyth(int mythNum, const unordered_set<string>& sunetIDs,
  map<int, int>& processCountMap, mutex& processCountMapLock) {

  int numProcesses = getNumProcesses(mythNum, sunetIDs);

  // If successful, add to the map and print out
  if (numProcesses >= 0) {
    processCountMapLock.lock();
    processCountMap[mythNum] = numProcesses;
    processCountMapLock.unlock();
    cout << oslock << "myth" << mythNum << " has this many CS110-student processes: " << numProcesses << endl << osunlock;
  }
}

/* Function: createCS110ProcessCountMap
 * ------------------------------------
 * Parameters:
 *    - sunetIDs: set of SUNET IDs for whom we should count processes
 *    - processCountMap: a map from myth number to CS110 process count that
 *                      we should fill in.
 * 
 * Returns: N/A
 * 
 * Fetches the CS110 student process count for each myth machine,
 * and adds the info to the provided map.  Uses a thread pool to parallelize.
 */
static void createCS110ProcessCountMap(const unordered_set<string>& sunetIDs,
          map<int, int>& processCountMap) {

  ThreadPool pool(kMaxNumSimultaneousThreads);
  mutex processCountMapLock;

  for (int mythNum = kMinMythMachine; mythNum <= kMaxMythMachine; mythNum++) {
    // Enqueue a task onto the pool to have it fetch this myth machine's count
    pool.schedule([mythNum, &sunetIDs, &processCountMap, &processCountMapLock]() {
      countCS110ProcessesForMyth(mythNum, sunetIDs, processCountMap, processCountMapLock);
    });
  }

  // Wait for all the queued tasks to complete
  pool.wait();
}

int main(int argc, char *argv[]) {
  // Create a set of student SUNETs
  unordered_set<string> cs110SUNETs;
  readStudentSUNETsFile(cs110SUNETs, kCS110StudentIDsFile);

  // Create a map from myth number -> CS110 process count and print its info
  map<int, int> processCountMap;
  createCS110ProcessCountMap(cs110SUNETs, processCountMap);
  printMythMachineWithFewestProcesses(processCountMap);

  return 0;
}
