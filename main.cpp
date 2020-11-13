#include <iostream>
#include <fstream>
#include <string>
#include "LexicalAnalyzer.h"
#include "GrammerAnalyzer.h"
#include "BasicBlock.h"  // ImCoder
#include "MipsGenerator.h"


using namespace std;

const string INFIFE = "testfile.txt";
const string OUTFILE = "output.txt";
const string ERRFILE = "error.txt";
const string IRFILE = "IMCode.txt";
const string TARGETFILE = "mips.txt";

int main() {

	ifstream fin(INFIFE);
	//ofstream fout(OUTFILE);
	//ofstream ferror(ERRFILE);
	ofstream f_ircode(IRFILE);
	ofstream f_targetcode(TARGETFILE);

	if (!fin.is_open()) {
		cout << "Could not open " << INFIFE << endl;
		exit(EXIT_FAILURE);
	}

	LexicalAnalyzer& myLexicalAnalyzer = LexicalAnalyzer::getInstance(fin);
	IMCode& myIMCode = IMCode::getInstance();
	GrammerAnalyzer& myGrammerAnalyzer = GrammerAnalyzer::getInstance(myLexicalAnalyzer, myIMCode);
	MipsGenerator& myMipsGenerator = MipsGenerator::getInstance(myIMCode);

	myLexicalAnalyzer.analyzeLexis();
	//myLexicalAnalyzer.show();
	myGrammerAnalyzer.analyzeGrammer();
	//myGrammerAnalyzer.show(fout);
	//myGrammerAnalyzer.showError(ferror);
	myIMCode.show_quaters(f_ircode);
	myMipsGenerator.generateMipsCode();
	myMipsGenerator.showMipsCode(f_targetcode);

	fin.close();
	//fout.close();
	//ferror.close();
	f_ircode.close();
	f_targetcode.close();

	return 0;
}