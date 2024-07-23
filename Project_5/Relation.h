#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <string>
#include <iostream>
#include <set>
#include <map>
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

Relation project(const std::vector<int>& projection) {
    std::vector<std::string> build_scheme;
    for (auto index : projection) {
        build_scheme.push_back(this->scheme.at(index));
    }
    Scheme new_scheme(build_scheme);
    Relation result(this->name, new_scheme); // Instantiating result Relation with the new scheme

    for (const auto& tup : this->tuples) {
        std::vector<std::string> projected_tuple_values;
        for (auto index : projection) {
            projected_tuple_values.push_back(tup.at(index));
        }
        Tuple new_tuple(projected_tuple_values);
        result.addTuple(new_tuple);
    }

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

  static bool joinable(const Scheme& leftScheme, const Scheme& rightScheme,
		       const Tuple& leftTuple, const Tuple& rightTuple){
          for (unsigned leftIndex = 0; leftIndex < leftScheme.size(); leftIndex++) {
            const std::string& leftName = leftScheme.at(leftIndex);
            const std::string& leftValue = leftTuple.at(leftIndex);
            //std::cout << "left name: " << leftName << " value: " << leftValue << std::endl;

            for (unsigned rightIndex = 0; rightIndex < rightScheme.size(); rightIndex++) {
            const std::string& rightName = rightScheme.at(rightIndex);
            const std::string& rightValue = rightTuple.at(rightIndex);
            //std::cout << "right name: " << rightName << " value: " << rightValue << std::endl;
            if((rightName == leftName) && (leftValue != rightValue)){
              return false;
            }
          }
          }
          

          return true;
           }
  
    
Relation join(const Relation& right) const {
    // Initialize the combined scheme with the left scheme.
    Scheme combinedScheme = this->scheme;

    // Map to store the index of right relation's attributes in the combined scheme.
    std::vector<int> rightIndicesMap(right.scheme.size(), -1);

    // Add unique attributes from the right scheme to the combined scheme.
    for (int i = 0; i < right.scheme.size(); ++i) {
        const std::string& attributeName = right.scheme.at(i);
        // Check if attributeName is not already in the combinedScheme.
        bool isUnique = true;
        for (int j = 0; j < this->scheme.size(); ++j) {
            if (this->scheme.at(j) == attributeName) {
                isUnique = false;
                rightIndicesMap[i] = j; // Map the index of the common attribute.
                break;
            }
        }
        if (isUnique) {
            combinedScheme.addName(attributeName);
            rightIndicesMap[i] = combinedScheme.size() - 1; // Update the mapping with the new index.
        }
    }

    // Create the result relation with the combined scheme.
    Relation result(this->name, combinedScheme);

   // Inside the join function, create new combined tuples
for (const Tuple& leftTuple : this->tuples) {
    for (const Tuple& rightTuple : right.tuples) {
        if (joinable(this->scheme, right.scheme, leftTuple, rightTuple)) {
            std::vector<std::string> newTupleValues;

            // First, add all values from the left tuple.
            for (int j = 0; j < leftTuple.size(); ++j) {
                newTupleValues.push_back(leftTuple.at(j));
            }

            // Then, for each attribute in the right scheme,
            // check if it is unique before adding the corresponding value from the right tuple.
            for (int i = 0; i < right.scheme.size(); ++i) {
                bool isUnique = true;
                for (int j = 0; j < this->scheme.size(); ++j) {
                    if (right.scheme.at(i) == this->scheme.at(j)) {
                        isUnique = false;
                        break; // Found a matching attribute name, so it's not unique.
                    }
                }
                if (isUnique) {
                    // This attribute from the right tuple doesn't exist in the left tuple's scheme,
                    // so we add its value to our new tuple.
                    newTupleValues.push_back(rightTuple.at(i));
                }
            }

            // Now, newTupleValues contains all values from the left tuple
            // plus the unique values from the right tuple.
            Tuple newTuple(newTupleValues);
            result.addTuple(newTuple);
        }
    }
}



    return result;
}




    bool isUnionCompatible(const Relation& other) const {
    // Check if the number of attributes is the same
    if (this->scheme.size() != other.scheme.size()) {
        return false;
    }
    
    // Check if the names (and optionally types) of the attributes are the same
    for (size_t i = 0; i < this->scheme.size(); ++i) {
        if (this->scheme.at(i) != other.scheme.at(i)) {
            // If there's a mismatch in names, the relations are not union-compatible
            return false;
        }
    }

    // If all checks passed, the relations are union-compatible
    return true;
}

bool unionWith(const Relation& other) {
    if (!isUnionCompatible(other)) {
        // Handle the incompatible schema case. This could be logging, throwing an exception, etc.
        // For now, simply return false or consider a different way to indicate an error.
        return false;
    }

    bool changed = false;
    for (const auto& tuple : other.tuples) {
        // Attempt to insert each tuple from 'other' into this relation.
        auto result = tuples.insert(tuple);
        if (result.second) { // If the tuple was newly added (not a duplicate).
            changed = true;
        }
    }

    return changed;
}


};
  

  
    
