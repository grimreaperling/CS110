/**
 * File: dinner-party.cc
 * ---------------------
 * Framework to exercise all of the 
 * functionality of the couple class.
 */

#include "couple.h"
#include <cassert>     // for assert macro
#include <iostream>    // for cout
using namespace std;

static const size_t kNumCouples = 3;

int main(int argc, char *argv[]) {
  couple hosts("Colleen", "Joe");
  assert(hosts.getFirst() == "Colleen");
  assert(hosts.getSecond() == "Joe");
  couple guests[kNumCouples];
  assert(guests[0].getFirst().empty());
  
  guests[0] = couple("Robin", "Jamie");
  guests[1] = couple("April", "Chris");
  guests[2] = couple("Jennifer", "Andrew");
  cout << "Dinner party guest list: " << endl;
  cout << "Hosts:  " << hosts << endl;
  for (const couple& c: guests) { // new for loop syntax with C++11, works because context provides the array length
    cout << "Guests: " << c << endl;
  }
  
  cout << endl;
  cout << "We're serving duck!" << endl;
  return 0;
}
