/**
 * File: xargs.cc
 * --------------
 * Provides a full version of the xargs executable, which simulates
 * a very constrained version of the xargs builtin.  Our xargs
 * tokens standard input around blanks and newlines, extends
 * the initial argument vector of its command to include
 * all of these extra tokens and then executes the full command.
 */

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include "unistd.h"
#include <sys/wait.h>
using namespace std;

static void pullAllTokens(istream& in, vector<string>& tokens) {
  while (true) {
    string line;
    getline(in, line);
    if (in.fail()) break;
    istringstream iss(line);
    while (true) {      
      string token;
      getline(iss, token, ' ');
      if (iss.fail()) break;
      tokens.push_back(token);
    }
  }
}

int main(int argc, char *argv[]) {
  vector<string> tokens;
  pullAllTokens(cin, tokens);
  char *xargsv[argc + tokens.size()];
  for (size_t i = 0; i < static_cast<size_t>(argc - 1); i++)
    xargsv[i] = argv[i + 1];
  for (size_t i = 0; i < tokens.size(); i++)
    xargsv[argc - 1 + i] = const_cast<char *>(tokens[i].c_str());
  xargsv[argc + tokens.size() - 1] = NULL;
  execvp(xargsv[0], xargsv);
  cerr << xargsv[0] << ": command not found, so xargs can't do its job!" << endl;
  return 0;
}
