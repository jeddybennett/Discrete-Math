#pragma once
#include <sstream>
#include <string>
#include <iostream>

//create enumerated types
enum TokenType{
	COMMA,PERIOD,Q_MARK, LEFT_PAREN, RIGHT_PAREN, COLON, COLON_DASH,
 MULTIPLY, ADD, SCHEMES, FACTS, RULES, QUERIES, ID, STRING,
 COMMENT, UNDEFINED, EOFILE
};

//define a class that identifies type names
class Token {
private:
TokenType type; 
std::string value;
int line;

public:
Token(TokenType type, std::string value, int line) : type(type), value(value), line(line) {}
std::string typeName() const{
	if (type == COMMA) return "COMMA";
	else if (type == PERIOD) return "PERIOD";
	else if (type == Q_MARK) return "Q_MARK";
	else if (type == LEFT_PAREN) return "LEFT_PAREN";
	else if (type == RIGHT_PAREN) return "RIGHT_PAREN";
	else if (type == COLON) return "COLON";
	else if (type == COLON_DASH) return "COLON_DASH";
	else if (type == MULTIPLY) return "MULTIPLY";
	else if (type == ADD) return "ADD";
	else if (type == SCHEMES) return "SCHEMES";
	else if (type == RULES) return "RULES";
	else if (type == QUERIES) return "QUERIES";
	else if (type == ID) return "ID";
	else if (type == STRING) return "STRING";
	else if (type == COMMENT) return "COMMENT";
	else if (type == UNDEFINED) return "UNDEFINED";
	else if (type == EOFILE) return "EOF";
	else if (type == FACTS) return "FACTS";
	//else return "INVALID TOKEN TYPE";
}

//output the token type, the token and line number
std::string toString() const{
std::stringstream out;
out << "(" << typeName() << "," << "\"" << value << "\"" << "," << line << ")";
return out.str();
}

TokenType getType() const{
	return type;
}

std::string getValue() const{
	return value;
}
};
