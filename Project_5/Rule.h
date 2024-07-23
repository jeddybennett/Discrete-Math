#pragma once
#include <sstream>
#include <string>
#include <iostream>
#include <stdexcept>
#include <vector>
#include "Token.h"
#include "Predicate.h"

class Rule{
private:
    Predicate headPredicate;
    std::vector<Predicate> bodyPredicates;

public:
    Rule(const Predicate& head) : headPredicate(head) {}

    void addBodyPredicate(const Predicate& predicate) {
        bodyPredicates.push_back(predicate);
    }

    Predicate getHeadPredicate() const{
        return headPredicate;
    }

    std::vector<Predicate> getBodyPredicate() const{
        return bodyPredicates;
    }

    std::string toString() const{
        std::stringstream ss;
        ss << headPredicate.toString() << " :- ";
        for(size_t i = 0; i < bodyPredicates.size(); ++i){
            if(i>0) ss << ",";
            ss << bodyPredicates[i].toString();
        }
        ss << ".";
        return ss.str();
    }

};
