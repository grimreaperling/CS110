/**
 * File: stl-algorithm-showcase.cc
 * -------------------------------
 * The STL includes a large number of templatized algorithms.
 * The STL takes a templated approach to generic programming, recognizing
 * that scores and scores of common, incredibly useful algorithms work
 * on homogenous sequences of data.
 *
 * The best online STL algorithm documentation I know resides under
 * http://en.cppreference.com, and in particular, look at:
 *
 *    http://en.cppreference.com/w/cpp/algorithm
 *
 * to gain a sense of what C++ provides off-the-shelf.
 */

#include <algorithm>
#include <numeric>
#include <cassert>
#include <iostream>
using namespace std;

static bool isEven(int number) { return number % 2 == 0; }
static bool isGreaterThan(int one, int two) { return one > two; }
static void assertIsOdd(int number) { assert(!isEven(number)); }

int main(int argc, char *argv[]) {
  int numbers[] = {11, 7, 19, 3, 17, 5, 2, 13};
  int *found = find(numbers, numbers + 8, 17);
  assert(found == numbers + 4 && *found == 17);
  found = find_if(numbers, numbers + 8, isEven);
  assert(found == numbers + 6 && *found == 2);
  assert(accumulate(numbers, numbers + 8, 0) == 77);
  sort(numbers, numbers + 8);
  assert(is_sorted(numbers, numbers + 8));
  assert(binary_search(numbers, numbers + 8, 11));
  assert(!binary_search(numbers, numbers + 8, 12));
  for_each(numbers + 1, numbers + 8, assertIsOdd);
  found = find(numbers, numbers + 8, 19);
  assert(found == numbers + 7 && *found == 19);
  found = lower_bound(numbers, numbers + 8, 17);
  assert(found == numbers + 6 && *found == 17);
  found = lower_bound(numbers, numbers + 8, 14);
  assert(found == numbers + 6 && *found == 17);
  reverse(numbers, numbers + 8);
  assert(is_sorted(numbers, numbers + 8, isGreaterThan));

  cout << "Made it to the end without asserting." << endl;
  return 0;
}
