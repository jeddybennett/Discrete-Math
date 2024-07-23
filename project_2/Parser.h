#pragma once
#include <sstream>
#include <string>
#include <iostream>
#include <stdexcept>
#include <vector>
#include "Token.h"
#include "Datalog.h"

class Parser {
 private:
  std::vector<Token> tokens;
  DatalogProgram datalog;
 public:
  Parser(const std::vector<Token>& tokens) : tokens(tokens) { }

TokenType tokenType() const{
    return tokens.at(0).getType();
  }
void parse(){
  try{
    datalogProgram();
  }
  catch(std::invalid_argument& e)
  {
    std::cout << "Failure!" << std::endl;
    std::cout<<e.what() << std::endl;
    return;
  }
  std::cout << "Success!" << std::endl;
  std::cout << datalog.toString() << std::endl;
}

void advanceToken(){
    tokens.erase(tokens.begin());
  }
void throwError(){
    throw std::invalid_argument(tokens.at(0).toString());
  }
void match(TokenType t){
  //match and consume the previous token
  if(tokens.at(0).getType() == t){
        advanceToken();
      }
  else{
    //throw an error if it's the incorrect token
       throwError();
      }
  }
  
Predicate headPredicate(){
  //get the value of the token
  std::string name = tokens.at(0).getValue();
    match(ID);

  //get the head predicate
    Predicate headPred(name);
    match(LEFT_PAREN);

  //check if the parameter list is empty
    if (tokenType() == RIGHT_PAREN) {
    throwError();
    }
    do {
       // Get the parameter value
        std::string paramName = tokens.at(0).getValue();

        headPred.addParameters(Parameter(paramName));
        advanceToken(); // Next token

        if (tokenType() == COMMA) advanceToken(); // Skip commas

    } while (tokenType() != RIGHT_PAREN);

    match(RIGHT_PAREN);

     // Return the head predicate
    return headPred;
}

Predicate predicate(){
  //get the value of the token
    std::string name = tokens.at(0).getValue();

  //match the ID and initialize a body predicate
    match(ID);
    Predicate bodyPred(name);
  
    match(LEFT_PAREN);
    if (tokenType() == RIGHT_PAREN) {
    throwError();
    }
    do {

    // Parameter value
    std::string paramName = tokens.at(0).getValue();
    bodyPred.addParameters(Parameter(paramName));

   //move to the next token
    advanceToken();

  //check for comma seperating parameters
    if (tokenType() == COMMA) advanceToken(); 
    } while (tokenType() != RIGHT_PAREN);
    match(RIGHT_PAREN);

   // Return the body predicate
    return bodyPred;
}

std::vector<Predicate> predicateList(){
  std::vector<Predicate> preds;
    while (tokenType() == COMMA) {
        match(COMMA);
        preds.push_back(predicate()); // Add each parsed predicate
    }
  return preds;
}


void parameter(){
  if (tokenType() == STRING){
    match(STRING);
  }
  else{
    match(ID);
  }
}

void parameterList(){
  if (tokenType() == COMMA){
    match(COMMA);
    parameter();
    parameterList();
  }
  else{
    //lambda
  }
}
void rule() {
 // Parse the head predicate
    Predicate head = headPredicate();
    match(COLON_DASH);
    std::vector<Predicate> body;

  // First body predicate
    body.push_back(predicate()); 

   // Additional body predicates
    std::vector<Predicate> additionalPreds = predicateList();
    body.insert(body.end(), additionalPreds.begin(), additionalPreds.end()); // Combine
    match(PERIOD);

    Rule newRule(head); // Construct the rule with head and body predicates
    for (const Predicate& pred : body) {
      newRule.addBodyPredicate(pred);
    }
    datalog.addRule(newRule); // Add the rule to the DatalogProgram
}

void ruleList(){
  if (tokenType() == ID){ 
  rule();
  ruleList();
  }
  else{
    //lambda
  }
}

void query(){
  Predicate queryPred = predicate();
  match(Q_MARK);
  datalog.addQuery(queryPred);
}

void queryList(){
  if (tokenType() == ID){ 
  query();
  queryList();
  }
  else{
    //lambda
  }
}

void idList() {
  if (tokenType() == COMMA) {
      match(COMMA);
      match(ID);
      idList();
    } 
    else {
      //lambda
    }
  }
void stringList(){
if (tokenType() == COMMA){
  match(COMMA);
  match(STRING);
  stringList();
}
else{
  //lambda
}
}
void scheme() {
  //get the value of the string
  std::string name = tokens.at(0).getValue();

  match(ID);
  match(LEFT_PAREN);

  //check if the parameters are empty
  if (tokenType() == RIGHT_PAREN) {
  throwError();
  }

  //set a boolean for the first parameter
  Predicate schemePredicate(name);
  bool isFirstParameter = true;
while (tokenType() != RIGHT_PAREN) {
    if (!isFirstParameter) {
        match(COMMA);//check for comma to seperate parameters
    } else {
        isFirstParameter = false; // The first parameter does not precede a comma
    }

    if (tokenType() != ID) {
        // If the current token is not an ID, throw an error or fail parsing
        throwError();
    }
    std::string paramName = tokens.at(0).getValue(); // Get the parameter's value assuming it's an ID
    match(ID); // This consumes the ID token, ensuring it's correctly identified as such
    schemePredicate.addParameters(Parameter(paramName));
}

match(RIGHT_PAREN);
datalog.addScheme(schemePredicate);
}

void schemeList(){
  if (tokenType() == ID) {
  scheme();
  schemeList();
  }
  else{
    //lambda
  }
}
void fact(){
std::string factName = tokens.at(0).getValue(); // Get the fact name
match(ID); // Match the fact name token
match(LEFT_PAREN);
if (tokenType() == RIGHT_PAREN) {
    throwError();
    }
Predicate factPredicate(factName);
bool isFirstParameter = true; // Track if we are on the first parameter

while (tokenType() != RIGHT_PAREN) {
    if (!isFirstParameter) {
        // For all parameters after the first, expect a comma before the next parameter
        match(COMMA);
    } else {
        isFirstParameter = false; // After the first parameter, this becomes false
    }

    std::string paramValue = tokens.at(0).getValue(); // Get the parameter value
    match(STRING); // Ensure that the parameter is of type STRING
    factPredicate.addParameters(Parameter(paramValue)); // Add the parameter
}

match(RIGHT_PAREN);
match(PERIOD);

datalog.addFact(factPredicate);
}
void factList(){
  if (tokenType() == ID){ 
    fact();
    factList();
}
else{
  //lambda
}
}

void datalogProgram(){
  //parse a textfile
  match(SCHEMES);
  match(COLON);

  scheme();
  schemeList();

  match(FACTS);
  match(COLON);

  factList();

  match(RULES);
  match(COLON);

  ruleList();

  match(QUERIES);
  match(COLON);

  query();
  queryList();

  match(EOFILE);
  }

};