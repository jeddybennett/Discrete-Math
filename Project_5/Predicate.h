#pragma once
#include <sstream>
#include <string>
#include <iostream>
#include <stdexcept>
#include <vector>
#include "Token.h"
#include "Parameter.h"

class Predicate{
private:
    std::string name;
    std::vector<Parameter> parameters;
public:
    Predicate(const std::string& name) : name(name) {}

    void addParameters(const Parameter& parameter){
        parameters.push_back(parameter);
    }
    std::string toString() const{
        std::stringstream ss;
        ss << name << "(";
        for(size_t i = 0; i < parameters.size(); ++i){
            if(i>0) ss << ',';
            ss<< parameters[i].toString();
        }
        ss << ')';
        return ss.str();
    }

    const std::vector<Parameter>& getParameters() const{
        return parameters;
    }

    const std::string& getName() const { 
        return name; 
        }
};
