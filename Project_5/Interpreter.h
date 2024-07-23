#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <stack>
#include "Datalog.h"
#include "Database.h"
#include "Relation.h"
#include "Tuple.h"
#include "Predicate.h"
#include "Scheme.h"
#include "Rule.h"
#include "Graph.h"

class Interpreter{

private:
    std::map<std::string, Relation> relations; // Use this map throughout the class
    DatalogProgram program;
    

public:
    Interpreter(const DatalogProgram& program): program(program){ 
        constructDatabase();
    }


void constructDatabase() {
        // Initialize relations with schemes
        for (const auto& scheme : program.getSchemes()) {
            std::vector<std::string> schemeAttributes;
            for (const auto& parameter : scheme.getParameters()) {
                schemeAttributes.push_back(parameter.toString()); // Directly using getValue assuming it returns the name or value
            }
            Scheme newScheme(schemeAttributes);
            Relation newRelation(scheme.getName(), newScheme);
            relations[scheme.getName()] = newRelation;
        }

        // Populate relations with facts
        for (const auto& fact : program.getFacts()) {
            std::vector<std::string> factValues;
            for (const auto& parameter : fact.getParameters()) {
                factValues.push_back(parameter.toString()); // Same here for fact values
            }
            Tuple newTuple(factValues);
            relations[fact.getName()].addTuple(newTuple);
        }
    }

    void evaluateQueries() {

    std::cout << std::endl;
    std::cout << "Query Evaluation" << std::endl;
    for (const auto& query : program.getQueries()) {
        auto relationIt = relations.find(query.getName());
        if (relationIt == relations.end()) continue;

        Relation resultRelation = relationIt->second; // Copy the relation for manipulation

        std::vector<int> projectIndices;
        std::vector<std::string> projectAttributes;
        std::map<std::string, int> variablePositions;

        for (size_t i = 0; i < query.getParameters().size(); ++i) {
            const auto& parameter = query.getParameters()[i];
            std::string parameterValue = parameter.toString();
            // Determine if the parameter is a constant by checking for quotes
            bool isConstant = parameterValue.front() == '\'';
            if (isConstant) {
                // Strip the quotes from the constant value before passing to select
                std::string unquotedValue = parameterValue.substr(1, parameterValue.length() - 2);
                resultRelation = resultRelation.select(i, unquotedValue);
            } else {
                // Handle variable
                if (variablePositions.find(parameterValue) == variablePositions.end()) {
                    variablePositions[parameterValue] = i;
                    projectIndices.push_back(i);
                    projectAttributes.push_back(parameterValue); // For rename operation
                } else {
                    // Select tuples where the value at this position matches the value at the first occurrence's position
                    resultRelation = resultRelation.select2(variablePositions[parameterValue], i);
                }
            }
        }

        // Project and rename based on unique variables
        resultRelation = resultRelation.project(projectIndices).rename(Scheme(projectAttributes));

        printQueryResult(query, resultRelation);
    }
}

void evaluateRules() {
    // Step 1: Construct the Dependency Graph
    Graph dependencyGraph = makeGraph(program.getRules());
    std::cout << "Dependency Graph" << std::endl;
    std::cout << dependencyGraph.toString() << "\n";
    
    std::cout << "Rule Evaluation" << std::endl;
    std::vector<std::set<int>> sccs = dependencyGraph.findSCCs();
    
    for (const auto& scc : sccs) {
        std::cout << "SCC: ";
        for (auto ruleIndexIt = scc.begin(); ruleIndexIt != scc.end(); ++ruleIndexIt) {
            if (ruleIndexIt != scc.begin()) {
                std::cout << ",";
            }
            std::cout << "R" << *ruleIndexIt;
        }
        std::cout << std::endl;

        bool changesMade = false;
        int iterations = 0;
        bool isSingleRuleSCC = scc.size() == 1;
        bool evaluatedOnce = false; // To prevent re-evaluation if SCC has only one rule and it's not recursive.

        // Check if it's a single-rule SCC and if the rule has a self-loop.
        bool hasSelfLoop = isSingleRuleSCC && dependencyGraph.hasEdge(*scc.begin(), *scc.begin());

        do {
            changesMade = false;
            for (int ruleIndex : scc) {
                // Skip the loop if it's a single-rule SCC without self-dependency and has been evaluated once.
                if (isSingleRuleSCC && !hasSelfLoop && evaluatedOnce) {
                    break;
                }
                
                const Rule& rule = program.getRules().at(ruleIndex);
                std::cout << rule.toString();

                std::vector<Relation> intermediateResults;
                for (const auto& predicate : rule.getBodyPredicate()) {
                    Relation tempRelation = evaluatePredicate(predicate);
                    intermediateResults.push_back(tempRelation);
                }

                // Join relations if there are multiple predicates.
                Relation joinedRelation = intermediateResults.size() > 0 ? intermediateResults[0] : Relation();
                for (size_t i = 1; i < intermediateResults.size(); ++i) {
                    joinedRelation = joinedRelation.join(intermediateResults[i]);
                }

                // Project columns based on the head predicate and rename relation.
                std::vector<int> projectIndices = getProjectIndices(rule.getHeadPredicate(), joinedRelation.getScheme());
                Relation projectedRelation = joinedRelation.project(projectIndices);
                const Relation& targetRelation = relations[rule.getHeadPredicate().getName()];
                Scheme targetScheme = targetRelation.getScheme();
                Relation renamedRelation = projectedRelation.rename(targetScheme);
                const auto preUnionTuples = targetRelation.getTuples();

                // Union with the database and check for changes.
                if (relations[rule.getHeadPredicate().getName()].unionWith(renamedRelation)) {
                    changesMade = true;
                    // Print the new tuples added by this rule evaluation
                    for (const auto& tuple : renamedRelation.getTuples()) {
                        if (preUnionTuples.find(tuple) == preUnionTuples.end()) {
                            std::cout << "  " << tuple.toString(targetScheme) << "\n"; 
                        }
                    }
                }

                evaluatedOnce = true; // Mark as evaluated once for single-rule SCC without self-dependency.
            }
            iterations++;
        } while (changesMade && (scc.size() > 1 || hasSelfLoop)); // Continue only if SCC has multiple rules or is recursive.

        std::cout << iterations << " passes: ";
        for (auto ruleIndexIt = scc.begin(); ruleIndexIt != scc.end(); ++ruleIndexIt) {
            if (ruleIndexIt != scc.begin()) {
                std::cout << ",";
            }
            std::cout << "R" << *ruleIndexIt;
        }
        std::cout << "\n";
    }
} // This is the correct ending of the function, the extra brace has been removed.




  static Graph makeGraph(const std::vector<Rule>& rules) {

    Graph graph(rules.size());

    for (size_t i = 0; i < rules.size(); ++i) {
        const Rule& fromRule = rules[i];
        //std::cout << "from rule R" << i << ": " << fromRule.toString() << std::endl;

        for (auto j: fromRule.getBodyPredicate()){
            //std::cout << "from body predicate: " << j.toString() << std::endl;

            for (size_t k = 0; k < rules.size(); ++k) {
                const Rule& toRule = rules[k];
                //std::cout << "to rule R" << k << ": " << toRule.toString() << std::endl;

                 // Assume headPredicate() returns the head Predicate of the rule
                if (j.getName() == toRule.getHeadPredicate().getName()) {
                    //std::cout << "dependency found: (R" << i << ",R" << k << ")" << std::endl;
                    graph.addEdge(i, k); // Assuming addEdge method exists
                }
            }
    }
  }
    

    return graph;
  }

private:
    void printQueryResult(const Predicate& query, const Relation& result) {
        std::cout << query.toString() << "? ";
        if (result.size() == 0) {
            std::cout << "No" << std::endl;;
        } else {
            std::cout << "Yes(" << result.size() << ")";
            if(result.size() > 1 || (result.size() == 1 && result.getScheme().size()!=0)){
                std::cout<<"\n";
            }
            for (const auto& tuple : result.getTuples()) {
                std::cout << "  ";
                for (size_t i = 0; i < tuple.size(); ++i) {
                    if (i > 0) std::cout << ", ";
                    std::cout << result.getScheme().at(i) << "=" << tuple.at(i);
                }
                std::cout << "\n";
            }
            
            
            
        }
        
    }

    Relation evaluatePredicate(const Predicate& predicate) {
    auto relationIt = relations.find(predicate.getName());
    if (relationIt == relations.end()) {
        throw std::runtime_error("Relation not found for predicate: " + predicate.getName());
    }

    Relation resultRelation = relationIt->second; // Make a copy of the relation for manipulation

    std::vector<int> projectIndices;
    std::vector<std::string> projectAttributes;
    std::map<std::string, int> variablePositions;

    for (size_t i = 0; i < predicate.getParameters().size(); ++i) {
        const auto& parameter = predicate.getParameters()[i];
        std::string parameterValue = parameter.toString(); // Assuming toString gives you the value directly
        bool isConstant = parameterValue.front() == '\''; // Assuming this checks if it's a constant

        if (isConstant) {
            std::string unquotedValue = parameterValue.substr(1, parameterValue.length() - 2);
            resultRelation = resultRelation.select(i, unquotedValue);
        } else {
            if (variablePositions.find(parameterValue) == variablePositions.end()) {
                variablePositions[parameterValue] = i;
                projectIndices.push_back(i);
                projectAttributes.push_back(parameterValue);
            } else {
                resultRelation = resultRelation.select2(variablePositions[parameterValue], i);
            }
        }
    }

    resultRelation = resultRelation.project(projectIndices).rename(Scheme(projectAttributes));
    return resultRelation;
}

std::vector<int> getProjectIndices(const Predicate& headPredicate, const Scheme& joinedScheme) {
    std::vector<int> indices;
    for (const auto& param : headPredicate.getParameters()) {
        std::string paramName = param.toString();
        bool found = false;
        for (size_t i = 0; i < joinedScheme.size(); ++i) {
            if (joinedScheme.at(i) == paramName) { // Ensure .at(i) is the correct way to access Scheme elements, and joinedScheme provides .at()
                indices.push_back(i);
                found = true;
                break; // Found the parameter name in the scheme, no need to continue
            }
        }
        if (!found) {
            std::cerr << "Error: Parameter name '" << paramName << "' not found in joined scheme.\n";
            // Consider how you want to handle this error. For now, just reporting.
        }
        }
std::cout << std::endl;
    return indices;
}

Scheme getSchemeFromPredicate(const Predicate& predicate) {
    std::vector<std::string> attributeNames;
    for (const auto& param : predicate.getParameters()) {
        // Assuming you can get a string representation of the parameter here.
        attributeNames.push_back(param.toString());
    }
    return Scheme(attributeNames);
}


};