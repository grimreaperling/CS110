/**
 * File: couple.h
 * --------------
 * This and its companion .cc file are here to illustrate
 * constructors, destructors, and operator<< to the extent you'll
 * need to work with them for this class. 
 * 
 * The couple class itself is trivial.
 */

#include <string>
#include <ostream>

class couple {
  friend std::ostream& operator<<(std::ostream& os, const couple& c);

 public:
  couple();
  couple(const std::string& one, const std::string& two);
  const std::string& getFirst() const;
  const std::string& getSecond() const;

 private:
  std::string first;
  std::string second;
};
