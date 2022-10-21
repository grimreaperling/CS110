/**
 * File: scrabble-word-finder.cc
 * -----------------------------
 * A standard program that outputs all of the words that can be formed from
 * the set of provided letters (which presumably come through as a string via
 * argv[1]).
 *
 * This program is not implemented as efficiently as possible.  The point
 * was to keep it short but to otherwise allow it to exist as a standalone
 * application that can be spawned off as a subprocess.
 */

#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#include <cassert>
#include <iostream>
#include "scrabble-word-finder.h"
#include "string-utils.h"
using namespace std;

/**
 * Function: isValidSet
 * --------------------
 * Returns true if and only if the specified string
 * consists of lowercase letters (and nothing else).
 */

static bool isValidSet(const string& letters) {
  for (char ch: letters) {
    if (!islower(ch)) 
      return false;
  }
  
  return true;
}

/**
 * Function: loadWords
 * -------------------
 * Pulls the contents of the wordsFiles (assumed to be a flat text
 * file of alphabetized English words, one per line) and populated
 * the supplied vector with the sorted sequence of words.
 */
static void loadWords(vector<string>& allWords, const string& wordsFile) {
  ifstream infile(wordsFile.c_str());
  assert(!infile.fail());
  allWords.clear();
  while (true) {
    string word;
    getline(infile, word, '\r');
    word = trim(word);
    if (word.empty()) break;
    allWords.push_back(word);
  }

  if (!is_sorted(allWords.cbegin(), allWords.cend())) 
    sort(allWords.begin(), allWords.end()); // technically unnecessary, but just making sure
}

/**
 * Function: publishAllPossibleWords
 * ---------------------------------
 * By consulting the sorted vector of lowercase English words, uses straightforward
 * recursion (with some sensible pruning) to list all of the words that can be
 * formed from using one or more of the letters in the the remaining set.
 */
static void publishAllPossibleWords(const string& remaining, const vector<string>& allWords, 
				    set<string>& formableWords, const string& soFar = "") {
  auto found = lower_bound(allWords.cbegin(), allWords.cend(), soFar);
  if (found == allWords.cend() || !startsWith(*found, soFar)) return;
  if (*found == soFar) formableWords.insert(soFar);  
  if (remaining.empty()) return;
  
  for (size_t i = 0; i < remaining.size(); i++) {
    publishAllPossibleWords(remaining.substr(0, i) + remaining.substr(i + 1),
			    allWords, formableWords, soFar + remaining[i]);
  }
}

/**
 * Serves at the entry point into the program that, given a set of 
 * lowercase letters, lists the set of all English words that can be formed
 * using one or more of the letters.
 */

static const size_t kMaxRackSize = 11;
static const size_t kMinRackSize = 2;
static const string kWordsFile = "words.txt";
int main(int argc, char *argv[]) {
  if (argc != 2) return kIncorrectArgumentCount;
  string letters = toLowerCase(argv[1]);
  if (letters.size() < kMinRackSize) return kCharacterSetTooShort;
  if (letters.size() > kMaxRackSize) return kCharacterSetTooLong;
  if (!isValidSet(letters)) return kInvalidCharacterSet;
  
  vector<string> allWords;
  loadWords(allWords, kWordsFile);
  set<string> formableWords;
  publishAllPossibleWords(letters, allWords, formableWords);
  for (const string& formableWord: formableWords) 
    cout << formableWord << endl;
  return 0;
}
