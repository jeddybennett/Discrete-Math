#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Parser.h"
#include "Scanner.h"
#include "Scheme.h"
#include "Tuple.h"
#include "Relation.h"
#include "Interpreter.h"
#include "Datalog.h"
#include "Database.h"
#include "Node.h"
#include "Graph.h"
#include "Rule.h"
using namespace std;



int main(int argc, char* argv[]) {
    string filename = argv[1];
    ifstream in;
    in.open(filename);
    stringstream ss;
    ss << in.rdbuf();
    string input = ss.str();
    in.close();
    Scanner S = Scanner(input);

    Parser p = Parser(S.Scan());
    p.parse();

    DatalogProgram datalogProgram = p.getDatalogProgram();
    //cout << datalogProgram.toString() << endl;
    Interpreter interpreter(datalogProgram);
    interpreter.evaluateRules();
    interpreter.evaluateQueries();

    // Node node;
    // node.addEdge(4);
    // node.addEdge(8);
    // node.addEdge(2);
    // cout << node.toString() << endl;

    // Graph graph(3);
    // graph.addEdge(1,2);
    // graph.addEdge(1,0);
    // graph.addEdge(0,1);
    // graph.addEdge(1,1);
    // cout << graph.toString();


  // predicate names for fake rules
  // first is name for head predicate
  // second is names for body predicates
//    pair<string,vector<string>> ruleNames[] = {
//     { "A", { "B", "C" } },
//     { "B", { "A", "D" } },
//     { "B", { "B" } },
//     { "E", { "F", "G" } },
//     { "E", { "E", "F" } },
//   };

//   vector<Rule> rules;

//   for (auto& rulePair : ruleNames) {
//     string headName = rulePair.first;
//     Rule rule = Rule(Predicate(headName));
//     vector<string> bodyNames = rulePair.second;
//     for (auto& bodyName : bodyNames)
//       rule.addBodyPredicate(Predicate(bodyName));
//     rules.push_back(rule);
//   }
//   // In your main or where you want to check the result:
// std::stack<int> finishStack;
//   Graph graph = Interpreter::makeGraph(rules);
//   cout << graph.toString() << '\n';

//   cout << graph.reverse().toString();
//   // graph.DFSForest(finishStack);

//   // Perform DFS Forest on the reversed graph
//     //std::stack<int> finishStack;
//     // Display the finish order from DFS Forest
//     cout << "DFS Forest Finish Order: ";
//     while (!finishStack.empty()) {
//         cout << finishStack.top() << " ";
//         finishStack.pop();
//     }
//     cout << endl;
}
