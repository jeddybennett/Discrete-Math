#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <string>
#include <iostream>


class Scheme {
private:

  std::vector<std::string> names;

public:
  Scheme() = default;
  
  Scheme(std::vector<std::string> names) : names(names) { }

  unsigned size() const {
    return names.size();
  }

  const std::string& at(int index) const {
    return names.at(index);
  }

  // TODO: add more delegation functions as needed

};