/**
 * File: vector-old-school.cc
 * --------------------------
 * I use this example just to illustrate how
 * the C++ STL vector is, in most ways, the same
 * thing as the CS106B Vector or the Java ArrayList,
 * with a slightly quirkier syntax.
 */

#include <vector>
#include <string>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <iomanip>
using namespace std;

static void printPrimesOne(const vector<int>& v) {
  cout << "Vector contents (take 1): " << endl << endl;
  for (size_t i = 0; i < v.size(); i++) {
    cout << "  " << setw(2) << (i + 1) << ".) " << v[i] << endl;
  }
  cout << endl;
}

static void printPrimesTwo(const vector<int>& v) {
  cout << "Vector contents (take 2): " << endl << endl;
  size_t label = 1;
  for (int prime: v) {
    cout << "  " << setw(2) << label << ".) " << prime << endl;
    label++;
  }
  cout << endl;
}

static void printPrimesThree(const vector<int>& v) {
  cout << "Vector contents (take 3): " << endl << endl;
  size_t label = 1;
  for_each(v.begin(), v.end(), [&label](int prime) -> void {
    cout << "  " << setw(2) << label << ".) " << prime << endl;
    label++;
  });
}

static const int kNumbers[] = {2, 3, 5, 7, 11, 13, 17, 21, 23, 27, 29, 31};
static const int kCount = sizeof(kNumbers)/sizeof(kNumbers[0]);

int main(int argc, char *argv[]) {
  vector<int> primes(kNumbers, kNumbers + kCount);
  primes.push_back(37);
  primes.push_back(41);
  
  vector<int>::const_iterator curr = primes.cbegin();
  assert(*curr == 2);
  ++curr; // prefix form is standard in STL-oriented C++
  assert(*curr == 3);  
  assert(is_sorted(primes.cbegin(), primes.cend()));
  
  // linear search
  assert(find(primes.cbegin(), primes.cend(), 14) == primes.cend());
  assert(find(primes.cbegin(), primes.cend(), 7) == primes.cbegin() + 3);
  
  // binary_search
  assert(binary_search(primes.cbegin(), primes.cend(), 41));
  assert(!binary_search(primes.cbegin(), primes.cend(), 33));
  assert(!binary_search(primes.cbegin() + 1, primes.cend() - 1, 41));
  
  auto found = lower_bound(primes.begin(), primes.end(), 19);
  assert(found == primes.begin() + 7 && *found == 21);
  auto iter = primes.insert(found, 19);
  assert(*iter == 19);
  primes.erase(iter + 1);

  assert(is_sorted(primes.cbegin(), primes.cend()));
  assert(binary_search(primes.cbegin(), primes.cend(), 19));
  assert(!binary_search(primes.cbegin(), primes.cend(), 21));

  printPrimesOne(primes);
  printPrimesTwo(primes);
  printPrimesThree(primes);
  
  cout << endl;
  cout << "Made it to the end without asserting." << endl;  
  return 0;
}
