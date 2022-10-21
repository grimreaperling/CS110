/**
 * Provides a C++ equivalent to Java's StreamTokenizer, which allows
 * the client to tokenize a collection of characters according to the 
 * set of delimiters as specified at construction time.
 */

#pragma once
#include <istream>
#include <string>

class StreamTokenizer {
 public:
  StreamTokenizer(std::istream& is, 
                  const std::string& delimiters, 
                  bool skipDelimiters = true);  
  bool hasMoreTokens() const;
  std::string nextToken();

 private:
  std::istream& is;
  std::string delimiters;
  mutable std::string savedChar;
  bool skipDelimiters;
  
  std::string getNextXMLChar() const;
  StreamTokenizer(const StreamTokenizer& orig) = delete;
  void operator=(const StreamTokenizer& other) = delete;
};

