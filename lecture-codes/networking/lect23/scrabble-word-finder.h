/**
 * File: scrabble-word-finder.h
 * ----------------------------
 * Defines a collection of constants returned by 
 * the scrabble-word-finder executable.
 */

#ifndef _scrabble_word_finder_
#define _scrabble_word_finder_

/**
 * Defines the collection of constants that need to be shared
 * and agreed upon by the scrabble-word-finder and the server that
 * relies on it as a subprocess (in the popen sense).
 */

static const int kIncorrectArgumentCount = 1;
static const int kInvalidCharacterSet = 2;
static const int kCharacterSetTooShort = 3;
static const int kCharacterSetTooLong = 4;
static const char *const kErrorMessages[kCharacterSetTooLong + 1] = {
  "No error",
  "Incorrect argument count",
  "Invalid characters included in letter set",
  "Letter set is too small",
  "Letter set is too large"
};

#endif
