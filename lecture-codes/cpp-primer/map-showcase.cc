/**
 * File: map-showcase.cc
 * ---------------------
 * Program that illustrates the complexities that come with populating
 * a map<string, int> with a histogram of word frequencies in epic novels.
 */

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cassert>
#include "stream-tokenizer.h"
#include "string-utils.h"
using namespace std;

static const string kDelimiters(" \t\n\r\b,./?><;:'\"[{]}\\=+-_)(*&^%$#@!~`1234567890");
static const int kWrongArgumentCount = 1;
static const int kFileNotAccessible = 2;

static void printLeadingWords(const string& header, const vector<pair<string, size_t> >& words, size_t count) {
  cout << header << ":" << endl;
  cout << endl;
  auto curr = words.cbegin();
  for (size_t i = 1; i <= count && curr != words.cend(); i++, ++curr) {
    cout << "  " << setw(2) << i << ".) " << curr->first << " [count: " << curr->second << "]" << endl;
  }
  cout << endl;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " <text-file>" << endl;
    return kWrongArgumentCount;
  }

  ifstream infile(argv[1]);
  if (infile.fail()) {
    cerr << "File named \"" << argv[1] << "\" couldn't be opened." << endl;
    return kFileNotAccessible;
  }

  map<string, size_t> m;
  StreamTokenizer st(infile, kDelimiters);
  while (st.hasMoreTokens()) {
    string str = toLowerCase(st.nextToken());
    m[str]++;
  }

  cout << "Ingested all of \"" << argv[1] << "\"." << endl;
  cout << "Discovered " << m.size() << " unique words." << endl;

  vector<pair<string, size_t>> alphabeticallySortedWords(m.cbegin(), m.cend());
  printLeadingWords("Leading words, sorted alphabetically", alphabeticallySortedWords, 15);
  vector<pair<string, size_t>> frequentlyOccurringWords(alphabeticallySortedWords);
  auto compfn = [](const pair<string, size_t>& one, const pair<string, size_t>& two) -> bool {
    return 
    (one.second > two.second) || 
    (one.second == two.second && one.first < two.first);
  };
  sort(frequentlyOccurringWords.begin(), frequentlyOccurringWords.end(), compfn);
  printLeadingWords("Leading words, sorted by frequency", frequentlyOccurringWords, 15);
  
  while (true) {
    cout << "Enter a word [or just hit enter to quit]: ";
    string word;
    getline(cin, word);
    word = toLowerCase(trim(word));
    if (word.empty()) break;
    auto found = m.find(word);
    if (found == m.end()) {
      cout << "The word \"" << word << "\" isn't present. #facepalm" << endl; 
    } else {
      cout << "The word \"" << word << "\" is present and appears this many times: " << found->second << " [with frequency rank: ";
      const pair<string, size_t>& match = *found;
      auto found = lower_bound(frequentlyOccurringWords.cbegin(), frequentlyOccurringWords.cend(), match, compfn);
      size_t rank = found - frequentlyOccurringWords.cbegin() + 1;
      cout << rank << "]" << endl;
    }
  }
  
  return 0;
}
