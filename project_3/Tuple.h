#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <string>
#include <iostream>
#include <set>
#include "Scheme.h"

class Tuple {

private:

  std::vector<std::string> values;

public:

  Tuple(std::vector<std::string> values) : values(values) { }

  unsigned size() const {
    return values.size();
  }

  const std::string& at(int index) const {
    return values.at(index);
  }

  bool operator<(const Tuple t) const {
    return values < t.values;
  }

  // TODO: add more delegation functions as needed

  std::string toString(const Scheme& scheme) const {
    const Tuple& tuple = *this;
    std::stringstream out;
    // fix the code to print "name=value" pairs
    for (size_t i = 0; i < scheme.size(); ++i) {
        if (i > 0) {
            out << ", "; // Add comma separator between pairs, but not before the first pair
        }
        out << scheme.at(i) << "=" << tuple.at(i);
    }
    // out << scheme.size();
    // out << scheme.at(0);
    // out << tuple.size();
    // out << tuple.at(0);
    return out.str();
  }

};