/**
 * File: couple.cc
 * ---------------
 * Implements the constructors, destructors, and trivial accessor
 * methods in the style I expect you to code.
 */

#include "couple.h"
using namespace std;

/**
 * Zero-arg constructor
 * --------------------
 * This does more work than you might think.  Because all couple objects
 * have two direct embedded string objects wedged inside of them, the
 * couple constructor calls the zero-arg constructor on each of the two
 * strings before it begins to execute any code within the curly braces.
 *
 * In general, it's always wise to have a zero-argument constructor unless
 * you know you'll never declare an collection of them (e.g. couple couples[100],
 * or vector<couple>).  If you define any other constructor at all (as wel have,
 * then you lose the default zero-argument one (which, had it existed, would have
 * been equivalent to what we're implementing right here).
 */

couple::couple() {}

/**
 * Two-argument constructor
 * ------------------------
 * Constructs a couple out of the two explicitly provided names.
 * In an ideal implementation, all of the work is done on an
 * initialization list.  The alternative is to exclude the initialization
 * list and instead assign first and second (using operator=) to be
 * copies of one and two.  But that initializes first and second to be
 * the empty string (because to omit a direct embedded object from the
 * initialization list is to zero-arg construct it) only to be updated
 * to be deep copies of one and two later on.
 */

couple::couple(const string& one, const string& two) : first(one), second(two) {}

/**
 * Obvious accessor methods that return references to existing strings
 * instead of copies.  The const in the return type tells the
 * caller that the string on the other end of the reference can't
 * (or at least shouldn't) be changed, and the const after the ()
 * means that the method implementation doesn't need to change the
 * object itself.
 */

const string& couple::getFirst() const { return first; }
const string& couple::getSecond() const { return second; }

/**
 * I will never make you implement your own operator<< function,
 * but I do so here because you'll see code that I give you that does.
 * We are allowed to overload operators in C++, and in this case we're
 * overloading << to have some sense as to how a couple should be
 * textually inserted into a stream (cout, cerr, an ofstream, etc).
 *
 * The implementation here isn't that important, and in fact it's somewhat
 * contrived.  But it still illustrates the how-does-it-work aspect of
 * overloaded operators.
 */

ostream& operator<<(ostream& os, const couple& c) {
  os << c.first << " and " << c.second;
  return os;
}
