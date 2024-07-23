#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <string>
#include <iostream>
#include <set>
#include <map>

class Node {

 private:

  std::set<int> adjacentNodeIDs;

 public:

  void addEdge(int adjacentNodeID) {
    adjacentNodeIDs.insert(adjacentNodeID);
  }

  const std::set<int>& getAdjacentNodeIDs() const {
        return adjacentNodeIDs;
    }
  std::string toString() const{
      std::stringstream ss;
        for (auto it = adjacentNodeIDs.begin(); it != adjacentNodeIDs.end(); ++it) {
            if (it != adjacentNodeIDs.begin()) {
                ss << ",";
            }
            ss << "R" << *it;
        }
        return ss.str();
    }
};