#include "Token.h"
#include "Scanner.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
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
	// Token t = S.scanToken();
	// cout << t.toString() << endl;
	S.Scan();
	return 0;
}
