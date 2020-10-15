#include <iostream>
#include <fstream>
#include <string>
#include "LexicalAnalyzer.h"
#include "GrammerAnalyzer.h"

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
	myLexicalAnalyzer.analyzeLexis();

	GrammerAnalyzer& myGrammerAnalyzer = GrammerAnalyzer::getInstance(myLexicalAnalyzer);
	myGrammerAnalyzer.analyzeGrammer();
	myGrammerAnalyzer.show(fout);

	fin.close();
	fout.close();

	return 0;
}