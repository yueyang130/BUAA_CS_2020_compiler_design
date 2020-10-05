#include "LexicalAnalyzer.h"
#include <cctype>



LexicalAnalyzer::LexicalAnalyzer(ifstream& inFile)  
	:fin(inFile), symList(INIT_LENGTH), symTypeList(INIT_LENGTH), symRowList(INIT_LENGTH)
{
	/*
	存储对ifstream的引用到fin，必须在初始化列表中初始化fin,在构造函数中的初始化无效
	*/

	rowCnt = 1;
	colCnt = 0;
	symCurr = "";
	needRetract = false;

	// 初始化keywords
	reserverMap.insert(make_pair("const", CONSTTK));
	reserverMap.insert(make_pair("int", INTTK));
	reserverMap.insert(make_pair("char", CHARTK));
	reserverMap.insert(make_pair("void", VOIDTK));
	reserverMap.insert(make_pair("main", MAINTK));
	reserverMap.insert(make_pair("if", IFTK));
	reserverMap.insert(make_pair("else", ELSETK));
	reserverMap.insert(make_pair("switch", SWITCHTK));
	reserverMap.insert(make_pair("case", CASETK));
	reserverMap.insert(make_pair("default", DEFAULTTK));
	reserverMap.insert(make_pair("while", WHILETK));
	reserverMap.insert(make_pair("for", FORTK));
	reserverMap.insert(make_pair("scanf", SCANFTK));
	reserverMap.insert(make_pair("printf", PRINTFTK));
	reserverMap.insert(make_pair("return", RETURNTK));
}

LexicalAnalyzer& LexicalAnalyzer::getInstance(ifstream &fin)
{
	static LexicalAnalyzer singleInstance(fin);
	return singleInstance;
}

void LexicalAnalyzer::nextChar() {
	if (!needRetract) {
		fin.get(chrCurr);
		if (isupper(chrCurr)) {
			chrCurr = tolower(chrCurr);
		}else if (chrCurr == '\n') {
			rowCnt++;
			colCnt = 0;
		} else if (chrCurr != EOF) {
			colCnt++;
		}
	} else {  
		// 需要回退时，将相当于下次读取的字符还是currChar
		needRetract = false;
	}
}


void LexicalAnalyzer::nextSym() {
	symCurr = ""; // 清空sym字符串
	nextChar();  // 预读一个字符

	while (isspace(chrCurr)) {  // 跳过空白
		nextChar();
	} 
	if (isalpha(chrCurr)) {	// 开头是字母，则只能是保留字或标识符
		while (isalpha(chrCurr)||isdigit(chrCurr)) {
			catToken();
			nextChar();
		}
		retract();
		if (!isReserver()) {  // 若是保留字，在函数中已经为symType设置了正确的值 
			symType = IDENFR;
		}
	}

	else if (isdigit(chrCurr)) {		// 判断是否是整形常量
		while (isdigit(chrCurr)) {
			catToken();
			nextChar();
		}
		retract();
		symType = INTCON;
	}

	else if (chrCurr == '\'') {   // 判断是否是字符常量
		do {
			nextChar();
			catToken();
		} while (isChr());

		if (chrCurr != '\'') {
			error();
		} else {
			symType = CHARCON;
		}
	}

	else if (chrCurr == '\"') { // 判断是否是字符串
		do {
			nextChar();
			catToken();
		} while (isInStr());

		if (chrCurr != '\"') {
			error();
		} else {
			symType = STRCON;
		}
	}

	else if (chrCurr == '=') {  // = or ==
		nextChar();
		if (chrCurr == '=') {
			catToken();
			symType = EQL;
		} else {
			symType = ASSIGN;
			retract();
		}
	}

	else if (chrCurr == '<') { // < or <=
		nextChar();
		if (chrCurr == '=') {
			catToken();
			symType = LEQ;
		} else {
			symType = LSS;
			retract();
		}
	}

	else if (chrCurr == '>') {  // > or >=
		nextChar();
		if (chrCurr == '=') {
			catToken();
			symType = GEQ;
		} else {
			symType = GRE;
			retract();
		}
	}

	else if (chrCurr == '!') {  // !=
		nextChar();
		if (chrCurr == '=') {
			catToken();
			symType = NEQ;
		} else {
			error();
		}
	}

	else if (chrCurr == '+') { symType = PLUS; catToken(); }
	else if (chrCurr == '-') { symType = MINUS; catToken(); }
	else if (chrCurr == '*') { symType = MULT; catToken(); }
	else if (chrCurr == '/') { symType = DIV; catToken(); }
	else if (chrCurr == ':') { symType = COLON; catToken(); }
	else if (chrCurr == ';') { symType = SEMICN; catToken(); }
	else if (chrCurr == ',') { symType = COMMA; catToken(); }
	else if (chrCurr == '(') { symType = LPARENT; catToken(); }
	else if (chrCurr == ')') { symType = RPARENT; catToken(); }
	else if (chrCurr == '[') { symType = LBRACK; catToken(); }
	else if (chrCurr == ']') { symType = RBRACK; catToken(); }
	else if (chrCurr == '{') { symType = LBRACE; catToken(); }
	else if (chrCurr == '}') { symType = RBRACE; catToken(); }
	else if (chrCurr == EOF) { ; }
	else { error(); }

}

bool LexicalAnalyzer::isReserver() {
	map<string, symbolType>::iterator iter = reserverMap.find(symCurr);
	if (iter != reserverMap.end()) {
		symType = iter->second;
		return true;
	} 
	return false;
}


void LexicalAnalyzer::analyze() {
	do {
		// TODO: continue to add exception handler
			nextSym();
			addInfo();
	} while (chrCurr != EOF);

}


void LexicalAnalyzer::show(ofstream& fout) {
	for (int i = 0; i < symList.size(); i++) {
		fout << symList[i] << " " << symTypeList[i] << endl;
	}
}