#pragma once
#include <vector>
#include <string>
#include <map>
#include "Scheme.h"
#include "Tuple.h"
#include "Relation.h"

class Database{

private:
std::map<std::string, Relation> database;

public:
 Database(const std::map<std::string, Relation>& database = {}) : database(database) { }
//add

void addRelation(const std::string& name, const Relation& relation){
        // Inserts a new relation or does nothing if the key already exists
    database.emplace(name, relation);
}

//find 
Relation* findRelation(const std::string& name){
    auto it = database.find(name);
    if (it != database.end()){
        return &it->second;
    }
    return nullptr;
}

};