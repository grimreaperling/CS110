/**
 * File: find.cc
 * -------------
 * Illustrates the notion of an STL algorithm, which typically
 * operate on data sequences that are defined by their begin and
 * their past-the-end iterators.
 *
 * Note that I will rarely if ever make you implement your own template
 * containers or algorithms in CS110.  This file presents an implementation
 * of a very simple algorithm that emulates the true behavior of the STL
 * find built-in.  I reimplement it here (and specifically exclude <algorithm>
 * from the list of #includes) so we can discuss how it works.
 */

#include <cassert>
#include <iostream>
using namespace std;

/**
 * Function: find
 * --------------
 * Defined for no other reason than to expose the implementation of a fairly
 * straightforward template algorithm in the style of an STL algorithm.
 */
template <typename Iterator, typename T>
Iterator find(Iterator begin, Iterator end, const T& elem) {
  Iterator curr = begin;
  while (curr != end && *curr != elem) {
    ++curr;
  }
  return curr;
}

int main(int argc, char *argv[]) {
  int numbers[] = {3, 1, 5, 8, 9, 5};
  assert(find(numbers, numbers + 6, 3) == numbers);
  assert(find(numbers, numbers + 6, 9) == numbers + 4);
  assert(find(numbers, numbers + 6, 5) == numbers + 2);
  assert(find(numbers, numbers + 6, 7) == numbers + 6);
  assert(find(numbers + 1, numbers + 3, 3) == numbers + 3);

  double gpas[] = {3.5, 3.1, 3.6, 3.2, 1.7, 3.0, 2.8, 3.1, 3.22};
  assert(find(gpas, gpas + 9, 3.6) == gpas + 2);
  assert(find(gpas, gpas + 9, 3.22) == gpas + 8);
  assert(find(gpas, gpas + 9, 1.1) == gpas + 9);
  assert(find(gpas + 4, gpas + 5, 1.7) == gpas + 4);

  string friends[] = {"Scott", "Jon", "Arlene", "David", "Tim", "Shana", "James", "Lisa"};
  assert(find(friends, friends + 8, "Shana") == friends + 5);
  
  cout << "Made it to the end without asserting." << endl;
  return 0;
}
