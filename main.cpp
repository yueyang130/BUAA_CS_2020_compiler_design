#include <iostream>
#include <fstream>
#include <string>
#include "LexicalAnalyzer.h"
using namespace std;

const string INFIFE = "testfile.txt";
const string OUTFILE = "output.txt";

int main() {

	ifstream fin(INFIFE);
	ofstream fout(OUTFILE);

	if (!fin.is_open()) {
		cout << "Could not open " << INFIFE << endl;
		exit(EXIT_FAILURE);
	}
	LexicalAnalyzer& myLexicalAnalyzer = LexicalAnalyzer::getInstance(fin);
	myLexicalAnalyzer.analyze();
	myLexicalAnalyzer.show(fout);

	fin.close();
	fout.close();

	return 0;
}