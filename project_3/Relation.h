#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <string>
#include <iostream>
#include <set>
#include "Scheme.h"
#include "Tuple.h"

class Relation {

private:

  std::string name;
  Scheme scheme;
  std::set<Tuple> tuples;

public:
Relation() = default;

Relation(const std::string& name, const Scheme& scheme)
    : name(name), scheme(scheme) { }

  void addTuple(const Tuple& tuple) {
    tuples.insert(tuple);
  }

std::string toString() const {
    std::stringstream out;
    // add code to print the Tuples, one per line

     for (const Tuple& tuple : tuples) { // Iterate over each tuple in the relation
        out << tuple.toString(scheme) << "\n"; // Convert each tuple to a string and add a newline
    }
    return out.str();
  }

Relation select(int index, const std::string& value) const {
    Relation result(name, scheme);
    std::string quotedValue = "'" + value + "'";
     for (const Tuple& tuple : tuples) { // Iterate over each tuple in the current relation
        if (tuple.at(index) == quotedValue) { // Check if the tuple's value at the given index matches the specified value
            result.addTuple(tuple); // If condition met, add the tuple to the result Relation
        }
    }
    return result;
  }

Relation select2(int index1, int index2) const{
  Relation result(name, scheme);

  for (const Tuple& tuple: tuples){
    if(tuple.at(index1) == tuple.at(index2)){
      result.addTuple(tuple);
    }
  }
  return result;
}

Relation project(std::vector<int> projection){
  Relation result(name, scheme); //need to change the scheme
  std::vector<std::string>build_scheme;
  for(auto index1: projection){
            build_scheme.push_back(scheme.at(index1));
      };
  Scheme new_scheme(build_scheme);
  for(auto tup: tuples){
        std::vector<std::string>tupl;
        for(auto index2: projection){
            tupl.push_back(tup.at(index2));
        };
        Tuple new_tuple(tupl);
        result.addTuple(new_tuple);
        tupl.clear();
  };
  return result;
  
}

Relation rename(Scheme new_scheme){
    Relation result(name, new_scheme);
    for (const Tuple& tuple : tuples) { // Iterate over each tuple in the current relation
          result.addTuple(tuple); // If condition met, add the tuple to the result Relation
        }
    return result;
    }

  int size() const {
        return tuples.size();
    }

    std::set<Tuple> getTuples() const {
        return tuples;
    }

    Scheme getScheme() const {
        return scheme;
    }
    

};