#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "Datalog.h"
#include "Database.h"
#include "Relation.h"
#include "Tuple.h"
#include "Predicate.h"
#include "Scheme.h"

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
    bool databaseChanged;
    int passCount = 0;
    std::cout << "Rule Evaluation" << "\n";
    do {
        databaseChanged = false;

        
        for (const auto& rule : program.getRules()) {
            // Log the rule being evaluated
            std::cout << rule.toString();
            // Evaluate predicates on the RHS of the rule.
            std::vector<Relation> intermediateResults;
            for (const auto& predicate : rule.getBodyPredicate()) { // Ensure this is the correct method name
                Relation tempRelation = evaluatePredicate(predicate);
                //std::cout << tempRelation.toString() << std::endl;
                intermediateResults.push_back(tempRelation);
            }


            //Join relations if there are multiple predicates.
            Relation joinedRelation = intermediateResults.size() > 0 ? intermediateResults[0] : Relation(); // Check if there are any predicates first
            for (size_t i = 1; i < intermediateResults.size(); ++i) {
                joinedRelation = joinedRelation.join(intermediateResults[i]);
            }

            //Project columns based on the head predicate.
            std::vector<int> projectIndices = getProjectIndices(rule.getHeadPredicate(), joinedRelation.getScheme());
            Relation projectedRelation = joinedRelation.project(projectIndices);

            //Rename relation to match the head predicate's scheme.
            const Relation& targetRelation = relations[rule.getHeadPredicate().getName()];
            Scheme targetScheme = targetRelation.getScheme();
            Relation renamedRelation = projectedRelation.rename(targetScheme);


            const auto preUnionTuples = targetRelation.getTuples();

            //Union with the database and check for changes.
            if (relations[rule.getHeadPredicate().getName()].unionWith(renamedRelation)) {
                databaseChanged = true;
                
                const auto& postUnionTuples = relations[rule.getHeadPredicate().getName()].getTuples();

                //Print the new tuples added by this rule evaluation
                for (const auto& tuple : renamedRelation.getTuples()) {
                    if (preUnionTuples.find(tuple) == preUnionTuples.end()) {
                        std::cout << "  "; 
                        std::cout << tuple.toString(targetScheme) << "\n"; 
                    }
                    
                }
            }
        }
        passCount++; // Moved increment here to count the initial pass as well
    } while (databaseChanged); // Repeat if the database changed during the last pass

    std::cout << "\nSchemes populated after " << passCount << " passes through the Rules.\n";
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