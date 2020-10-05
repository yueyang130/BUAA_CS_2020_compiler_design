#include "LexicalAnalyzer.h"
#include <cctype>



LexicalAnalyzer::LexicalAnalyzer(ifstream& inFile)  
	:fin(inFile), symList(INIT_LENGTH), symTypeList(INIT_LENGTH), symRowList(INIT_LENGTH)
{
	/*
	�洢��ifstream�����õ�fin�������ڳ�ʼ���б��г�ʼ��fin,�ڹ��캯���еĳ�ʼ����Ч
	*/

	rowCnt = 1;
	colCnt = 0;
	symCurr = "";
	needRetract = false;

	// ��ʼ��keywords
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
		// ��Ҫ����ʱ�����൱���´ζ�ȡ���ַ�����currChar
		needRetract = false;
	}
}


void LexicalAnalyzer::nextSym() {
	symCurr = ""; // ���sym�ַ���
	nextChar();  // Ԥ��һ���ַ�

	while (isspace(chrCurr)) {  // �����հ�
		nextChar();
	} 
	if (isalpha(chrCurr)) {	// ��ͷ����ĸ����ֻ���Ǳ����ֻ��ʶ��
		while (isalpha(chrCurr)||isdigit(chrCurr)) {
			catToken();
			nextChar();
		}
		retract();
		if (!isReserver()) {  // ���Ǳ����֣��ں������Ѿ�ΪsymType��������ȷ��ֵ 
			symType = IDENFR;
		}
	}

	else if (isdigit(chrCurr)) {		// �ж��Ƿ������γ���
		while (isdigit(chrCurr)) {
			catToken();
			nextChar();
		}
		retract();
		symType = INTCON;
	}

	else if (chrCurr == '\'') {   // �ж��Ƿ����ַ�����
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

	else if (chrCurr == '\"') { // �ж��Ƿ����ַ���
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