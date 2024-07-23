#pragma once
#include <sstream>
#include <string>
#include <iostream>
#include <stdexcept>
#include <vector>
#include "Token.h"

//create a scanner class
class Scanner {
	private:
		std::string input;
		int line = 1;
	public:
		Scanner(const std::string& input) : input(input) {}

	
	void Scan() {
	std::vector<Token> totalTokens;
	while(!input.empty()){
			Token token = scanToken();
			totalTokens.push_back(token);
			
	}

	if(input.empty() && ((totalTokens.size() == 0) || totalTokens[totalTokens.size() - 1].typeName() != "EOF")){
		
		totalTokens.push_back(Token(EOFILE, "", line));
	}
	for(const Token& t: totalTokens){
		std::cout<<t.toString()<<std::endl;
	}
	std::cout<<"Total tokens = " << totalTokens.size()<<std::endl;
	// return totalTokens;
	}
	


	//define function to identify the tokens in the input
	Token scanToken(){
		TokenType type;
		std::string value;
		


		//
		//remove the whitespace
		while (!input.empty() && isspace(input.at(0))){
			if (input.at(0) == '\n')
			{
				line ++;
			}
			input = input.substr(1);
	}
	if (input.empty()){
		type = EOFILE;
		value = "";
		return Token(type, value, line);
	}
	//comma isn't working
	if (input.at(0) == ','){
		type = COMMA;
		value = ",";
		input = input.substr(1);
		return Token(type, value, line);
	}
	else if(input.at(0) == '.'){
		type = PERIOD;
		value = ".";
		input = input.substr(1);
		return Token(type, value, line);
	}
	else if(input.at(0) == '('){
		type = LEFT_PAREN;
		value = "(";
		input = input.substr(1);
		return Token(type, value, line);
	}
	else if(input.at(0) == ')'){
		type = RIGHT_PAREN;
		value = ")";
		input = input.substr(1);
		return Token(type, value, line);
	}
	else if(input.at(0) == '?'){
		type = Q_MARK;
		value = "?";
		input = input.substr(1);
		return Token(type, value, line);
	}
	else if(input.at(0) == '*'){
		type = MULTIPLY;
		value = "*";
		input = input.substr(1);
		return Token(type, value, line);
	}
	else if(input.at(0) == '+'){
		type = ADD;
		value = "+";
		input = input.substr(1);
		return Token(type, value, line);
	}
	else if(input.substr(0,7) == "Schemes" && ((input.length()==7) || !isalnum(input.at(7)))){
		type = SCHEMES;
		value = "Schemes";
		input = input.substr(7);
		return Token(type, value, line);
	}
	else if(input.substr(0,5) == "Facts" && ((input.length()==5) || (!isalnum(input.at(5))))){
		type = FACTS;
		value = "Facts";
		input = input.substr(5);
		return Token(type, value, line);
	}
	else if(input.substr(0,7) == "Queries" && (input.length()==7 || !isalnum(input.at(7)))){
		type = QUERIES;
		value = "Queries";
		input = input.substr(7);
		return Token(type, value, line);
	}
	else if(input.substr(0,5) == "Rules" && ((input.length()==5) || (!isalnum(input.at(5))))){
		type = RULES;
		value = "Rules";
		input = input.substr(5);
		return Token(type, value, line);
	}
	
	else if(input.length() >= 2 && input.at(0) == ':' && input.at(1) == '-'){
		type = COLON_DASH;
		value = ":-";
		input = input.substr(2);
		return Token(type, value, line);
	}
	//colon isn't working
	else if(input.at(0) == ':'){
		type = COLON;
		value = ":";
		input = input.substr(1);
		return Token(type, value, line);
	}
	//unclosed string is not working and it's not returning the last quotation mark 
	else if(input.at(0) == '\''){
		int pos = 1;
		int linecount = 0;
		while (pos < input.length() && (input.at(pos) != '\'') || 
					(pos < input.length() && input.at(pos) == '\'' && pos + 1 < input.length() && input.at(pos + 1) == '\''))
			{
				if (input.at(pos) == '\'' && input.at(pos + 1) == '\''){
					pos++;
				}
				if (input.at(pos) == '\n'){
					linecount ++;
				}
				pos++;
			}
		
		if(pos >= input.length()){
			type = UNDEFINED;
			value = input;  // Take the rest of the input as the comment
			input = input.substr(input.length()); 
			line+=linecount;
		}
		else{
		type = STRING;
		value = input.substr(0, pos+1);
		input = input.substr(pos+1);
		line += linecount;
		}
		return Token(type, value, line - linecount);
	}
	else if(input.length()>=2 && input.at(0) == '#' && input.at(1) != '|'){
		int endOfComment = input.find('\n');
    	if (endOfComment == std::string::npos) {
        // The comment goes until the end of the file.
        	endOfComment = input.length();
   		}
		type = COMMENT;
		value = input.substr(0, endOfComment);
		input = input.substr(endOfComment);
		return Token(type, value, line);
	}
	else if(input.at(0) == '#' && input.at(1) == '|'){
		int pos = input.find('|#', 2);
		int linecount = 0;
		for(int i = 0; i < input.length(); i++){
			if(input[i] == '|' && input[i+1] == '#'){
				break;
			}
			if(input[i] == '\n'){
				linecount ++;
			}
		}
    	if (pos == std::string::npos) {
			type = UNDEFINED;
			value = input;  // Take the rest of the input as the comment
			input = input.substr(input.length()); 
			line += linecount;
   		}
		else{
		type = COMMENT;
		value = input.substr(0, pos+1);
		input = input.substr(pos+1);
		line += linecount;
		}
		return Token(type, value, line - linecount);
	}

	//Identifier isn't currently working
	else if(isalpha(input.at(0))){
		int index = 1;
		while(index < input.length() && isalnum(input.at(index))){
			index++;
		}
		std::string PotentialID = input.substr(0, index);
		if(PotentialID == "Schemes" || PotentialID == "Facts" || PotentialID == "Rules" || PotentialID == "Queries"){
			if(PotentialID == "Schemes"){
			type = SCHEMES;
			}
			else if(PotentialID == "Facts"){
			type = FACTS;
			}
			else if(PotentialID == "Rules"){
			type = RULES;
			}
			else if(PotentialID == "Queries"){
			type = QUERIES;
			}
		}
		else{
			type = ID;
		}
		value = PotentialID;
		input = input.substr(index);
		return Token(type, value, line);
	}
	
	//otherwise state that the token is invalid
	else {
		type = UNDEFINED;
		value = input.substr(0,1);
		input = input.substr(1);
		return Token(type, value, line);
	}
	}
};



