#pragma once
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
#include "Parser.h"
#include "Scanner.h"

using namespace std;


int main(int argc, char* argv[]){
        string filename = argv[1];
        ifstream in;
        in.open(filename);
        stringstream ss;
        ss << in.rdbuf();
        string input = ss.str();
        in.close();
        
  Scanner S = Scanner(input);

  Parser p = Parser(S.Scan());
  p.parse();


};

