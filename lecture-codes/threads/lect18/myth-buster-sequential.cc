/**
 * File: myth-buster-sequential.cc
 * -------------------------------
 * A sequential (and very slow) program that serializes
 * several network calls to find out how many CS110 student processes
 * are running on each of the myth machines.
 */

#include <unistd.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <fstream>
#include <unordered_set>
#include <map>
#include <vector>
#include <algorithm>
#include "myth-buster-service.h"
#include "string-utils.h"
using namespace std;

// Filename containing student sunets, 1 per line
static const string kCS110StudentIDsFile = "studentsunets.txt";

// The range of myth machine numbers to poll
static const int kMinMythMachine = 51;
static const int kMaxMythMachine = 66;

// The exit status to use if the student SUNETs file can't be read
static const int kFileInaccessible = 1;


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

/* Function: createCS110ProcessCountMap
 * ------------------------------------
 * Parameters:
 *    - sunetIDs: set of SUNET IDs for whom we should count processes
 *    - processCountMap: a map from myth number to CS110 process count that
 *                      we should fill in.
 * 
 * Returns: N/A
 * 
 * Fetches the CS110 student process count sequentially for each myth machine,
 * and adds the info to the provided map.
 */
static void createCS110ProcessCountMap(const unordered_set<string>& sunetIDs,
          map<int, int>& processCountMap) {

  for (int mythNum = kMinMythMachine; mythNum <= kMaxMythMachine; mythNum++) {
    int numProcesses = getNumProcesses(mythNum, sunetIDs);

    // If successful, add to the map and print out
    if (numProcesses >= 0) {
      processCountMap[mythNum] = numProcesses;
      cout << "myth" << mythNum << " has this many CS110-student processes: " << numProcesses << endl;
    }
  }
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
