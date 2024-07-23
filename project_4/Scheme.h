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

  void addName(const std::string& name) {
        names.push_back(name);
    }

  const std::string& getName(size_t index) const {
        if (index < names.size()) {
            return names[index];
        }
        throw std::out_of_range("Scheme index out of range");
    }
};