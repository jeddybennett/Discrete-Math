#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <string>
#include <iostream>
#include <stack>
#include <set>
#include <map>
#include "Node.h"

class Graph {

 private:

  std::map<int,Node> nodes;

 public:
  Graph(int size) {
    for (int nodeID = 0; nodeID < size; nodeID++)
      nodes[nodeID] = Node();
  }

  void addEdge(int fromNodeID, int toNodeID) {
    nodes[fromNodeID].addEdge(toNodeID);
  }

  std::string toString() const {
        std::stringstream ss;
        for (const auto& pair : nodes) {
            int nodeID = pair.first;
            const Node& node = pair.second;
            ss << "R" << nodeID << ":" << node.toString() << "\n";
        }
        return ss.str();
  }

  Graph reverse() const {
    Graph reversedGraph(nodes.size());
    for (const auto& pair : nodes) {
        int fromNodeID = pair.first;
        const Node& node = pair.second;
        for (int toNodeID : node.getAdjacentNodeIDs()) {
            reversedGraph.addEdge(toNodeID, fromNodeID);
        }
    }
    return reversedGraph;
}

void DFS(int nodeId, std::map<int, bool>& visited, std::stack<int>& finishStack) {
    visited[nodeId] = true;
    for (int adjNode : nodes[nodeId].getAdjacentNodeIDs()) {
        if (!visited[adjNode]) {
            DFS(adjNode, visited, finishStack);
        }
    }
    finishStack.push(nodeId);
}

void DFSForest(std::stack<int>& finishStack) {
    std::map<int, bool> visited;
    for (const auto& node : nodes) {
        visited[node.first] = false;
    }

    for (const auto& node : nodes) {
        if (!visited[node.first]) {
            DFS(node.first, visited, finishStack);
        }
    }
}


void DFSUtil(int v, std::map<int, bool>& visited, std::set<int>& currentSCC) {
    visited[v] = true;
    currentSCC.insert(v);

    // Directly access the adjacent nodes from the `nodes` map
    const std::set<int>& adj = nodes[v].getAdjacentNodeIDs();
    for (int i : adj) {
        if (!visited[i]) {
            DFSUtil(i, visited, currentSCC);
        }
    }
}



std::vector<std::set<int>> findSCCs() {
    std::vector<std::set<int>> sccs;
    std::stack<int> Stack;

    // Create the reversed graph and perform DFSForest to fill Stack with nodes in post-order
    Graph reversedGraph = this->reverse();
    reversedGraph.DFSForest(Stack);

    // Prepare for the second DFS to find SCCs
    std::map<int, bool> visited;
    for (const auto& node : nodes) {
        visited[node.first] = false;
    }

    // Process all nodes in the order defined by Stack to identify SCCs
    while (!Stack.empty()) {
        int v = Stack.top();
    
        if (!visited[v]) {
            std::set<int> currentSCC;
            this->DFSUtil(v, visited, currentSCC); // Use this graph, not the reversed for SCC discovery
            sccs.push_back(currentSCC);
        }
        Stack.pop();
    }

    return sccs;
}

bool hasEdge(int fromNodeID, int toNodeID) const {
    if (nodes.find(fromNodeID) == nodes.end()) {
        return false; // No such node exists
    }
    const Node& fromNode = nodes.at(fromNodeID);
    const std::set<int>& adjNodeIDs = fromNode.getAdjacentNodeIDs();
    return adjNodeIDs.find(toNodeID) != adjNodeIDs.end();
}

};
