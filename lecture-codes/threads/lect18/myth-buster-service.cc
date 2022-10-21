/**
 * File: myth-buster-service.cc
 * ----------------------------
 * Provides the *very* hacky implementation of the 
 * getNumProcesses function, which just uses popen (similar to our
 * subprocess function) to ssh to the specified
 * myth machine, and run a remote ps aux to count processes.
 *
 * You are not reading this.
 */

#include "myth-buster-service.h"
#include <unistd.h>
#include <sstream>
#include <cstring>
#include "string-utils.h"
#include "thread-utils.h"
using namespace std;

int getNumProcesses(int num, const unordered_set<string>& sunetIDs) {
  ostringstream oss;
  oss << "ssh -o ConnectTimeout=2 -q myth" << num << " ps aux --noheaders | awk '{ print $1\"\\n\"}'";
  FILE *instream = popen(oss.str().c_str(), "r"); // popen is like our subprocess routine
  int numProcesses = 0;
  int numCS110Processes = 0;
  while (true) {
    char output[1024];
    memset(output, 0, sizeof(output));
    if (fscanf(instream, "%1023[^\n]\n", output) == -1) break;
    numProcesses++;
    string sunetID(output);
    sunetID = trim(sunetID);
    if (sunetIDs.find(sunetID) != sunetIDs.cend()) 
      numCS110Processes++;
  }
  pclose(instream);
  if (numProcesses == 0) return -1; // we didn't really connect to it (HACK!)
  return numCS110Processes;
}
