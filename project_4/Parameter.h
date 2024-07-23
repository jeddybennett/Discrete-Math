#pragma once
#include <sstream>
#include <string>
#include <iostream>
#include <stdexcept>
#include <vector>
#include "Token.h"

class Parameter{
private:
    std::string name;
    bool isID = true;
public:
    Parameter(const std::string& name) : name(name){
        if(name.at(0) == '\''){
            isID = false;
        }
    }
    bool getisID() const{
        return isID;
    }
    std::string toString() const{
        return name;
    }
};