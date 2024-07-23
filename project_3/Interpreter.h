#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "Datalog.h"
#include "Database.h"
#include "Relation.h"
#include "Tuple.h"
#include "Predicate.h"

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

private:
    void printQueryResult(const Predicate& query, const Relation& result) {
        std::cout << query.toString() << "? ";
        if (result.size() == 0) {
            std::cout << "No" << std::endl;;
        } else {
            std::cout << "Yes(" << result.size() << ")\n";
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

};