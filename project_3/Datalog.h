#pragma once
#include <sstream>
#include <string>
#include <iostream>
#include <set>
#include <stdexcept>
#include <vector>
#include "Token.h"
#include "Rule.h"
#include "Predicate.h"

class DatalogProgram{
private:
    std::vector<Predicate> schemes;
    std::vector<Predicate> facts;
    std::vector<Rule> rules;
    std::vector<Predicate> queries;
    std::set<std::string> domain;

public:
    void addScheme(const Predicate& scheme) { 
        schemes.push_back(scheme);
    }

    void addFact(const Predicate& fact) { 
        facts.push_back(fact);

        for(const auto& parameter: fact.getParameters()){
            domain.insert(parameter.toString());
        }
    }

    void addRule(const Rule& rule) { 
        rules.push_back(rule);
    }

    void addQuery(const Predicate& query) { 
        queries.push_back(query);
    }

    const std::vector<Predicate>& getSchemes() const{
        return schemes;
    }

    const std::vector<Predicate>& getFacts() const{
        return facts;
    }

    const std::vector<Rule>& getRules() const{
        return rules;
    }

    const std::vector<Predicate>& getQueries() const{
        return queries;
    }

    const std::set<std::string>& getDomain() const{
        return domain;
    }

    std::string toString() const{
        std:: stringstream ss;
        ss << "Schemes(" << schemes.size()<< "):\n";
        for (const auto& scheme: schemes){
            ss << " " << scheme.toString() << std::endl;
        }
            ss << "Facts(" << facts.size() << "):\n";
            for (const auto& fact : facts) {
                ss << "  " << fact.toString() << "." << std::endl;
            }

            ss << "Rules(" << rules.size() << "):\n";
            for (const auto& rule : rules) {
                ss << "  " << rule.toString() << std::endl;
            }

        ss << "Queries(" << queries.size() << "):\n";
        for (const auto& query : queries) {
            ss << "  " << query.toString() << "?" << std::endl;
        }

        ss << "Domain(" << domain.size() << "):\n";
        for (const auto& value : domain) {
            ss << "  " << value << "" << std::endl;
        }

        return ss.str();
    }

};