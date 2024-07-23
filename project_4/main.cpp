#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "Parser.h"
#include "Scanner.h"
#include "Scheme.h"
#include "Tuple.h"
#include "Relation.h"
#include "Interpreter.h"
#include "Datalog.h"
#include "Database.h"
using namespace std;



int main(int argc, char* argv[]) {
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

    DatalogProgram datalogProgram = p.getDatalogProgram();
    //cout << datalogProgram.toString() << endl;
    Interpreter interpreter(datalogProgram);
    interpreter.evaluateRules();
    interpreter.evaluateQueries();

}