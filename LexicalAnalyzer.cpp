#include "LexicalAnalyzer.h"
#include <cctype>
#include <algorithm>


LexicalAnalyzer::LexicalAnalyzer(istream& inFile)  
	:fin(inFile)
{
	/*
	存储对ifstream的引用到fin，必须在初始化列表中初始化fin,在构造函数中的初始化无效
	*/
	sym_infor_list_.reserve(INIT_LENGTH);
	 
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

LexicalAnalyzer& LexicalAnalyzer::getInstance(istream &fin)
{
	static LexicalAnalyzer singleInstance(fin);
	return singleInstance;
}

void LexicalAnalyzer::nextChar() {
	if (!needRetract) {
		chrCurr = fin.get();   // fin.get(chr)无法将EOF（-1）读入chr中，必须使用chr = fin.get()
		if (chrCurr == '\n') {
			rowCnt++;
			colCnt = 0;
		}
	} else {
		// 需要回退时，将相当于下次读取的字符还是currChar
		needRetract = false;
	}
}


void LexicalAnalyzer::nextSym() {
	symCurr = ""; // 清空sym字符串

	while (isspace(chrCurr)) {  // 跳过空白
		nextChar();
	} 
	if (isLetter()) {	// 开头是字母，则只能是保留字或标识符
		while (isLetter()||isdigit(chrCurr)) {
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
		
		nextChar();
		if (isChr()) { catToken(); }
		else { 
			error();
		}
		
		nextChar();
		if (chrCurr != '\'') {
			error();
		} else {
			symType = CHARCON;
		}
	}

	else if (chrCurr == '\"') { // 判断是否是字符串
		int str_len = 0;
		do {
			nextChar();
			if (isInStr()) { catToken(); str_len++; }
			else if (chrCurr != '\"') { error("字符串中有非法字符"); str_len++; }
		} while (chrCurr != '\"');
		if (str_len == 0) { error("空字符串"); }

		symType = STRCON;
	}

	else if (chrCurr == '=') {  // = or ==
		catToken();
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
		catToken();
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
		catToken();
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
		catToken();
		nextChar();
		if (chrCurr == '=') {
			catToken();
			symType = NEQ;
		} else {
			error();
		}
	}

	else if (chrCurr == '+') { symType = PLUS; catToken(); }
	else if (chrCurr == '-') { symType = MINU; catToken(); }
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
	else if (chrCurr == EOF) { return; }
	else { error();  }
	
	// 当没有中途返回，说明读取sym成功，记录到列表中
	addInfo();

}

bool LexicalAnalyzer::isReserver() {
	string lowSym = symCurr;
	transform(lowSym.begin(), lowSym.end(), lowSym.begin(), ::tolower);

	map<string, symbolType>::iterator iter = reserverMap.find(lowSym);
	if (iter != reserverMap.end()) {
		symType = iter->second;
		return true;
	} 
	return false;
}

void LexicalAnalyzer::analyzeLexis() {
	nextChar();  // 预读一个字符
	while (fin) {  // cin.get无法读取EOF到chrCurr，但是bool（cin）可以表示是否能继续读取 
		// TODO: continue to add exception handler
		nextSym();
		nextChar();
	}  

}


void LexicalAnalyzer::show(ostream& fout) {
	for (int i = 0; i < sym_infor_list_.size(); i++) {
		fout <<  type_to_str(sym_infor_list_[i].type) << " " << sym_infor_list_[i].sym<< endl;
	}
}
string LexicalAnalyzer::type_to_str(symbolType type) {
	switch (type) {
	case IDENFR:
		return "IDENFR";
	case INTCON:
		return "INTCON";
	case CHARCON:
		return "CHARCON";
	case STRCON:
		return "STRCON";
	case CONSTTK:
		return "CONSTTK";
	case INTTK:
		return "INTTK";
	case CHARTK:
		return "CHARTK";
	case VOIDTK:
		return "VOIDTK";
	case MAINTK:
		return "MAINTK";
	case IFTK:
		return "IFTK";
	case ELSETK:
		return "ELSETK";
	case SWITCHTK:
		return "SWITCHTK";
	case CASETK:
		return "CASETK";
	case DEFAULTTK:
		return "DEFAULTTK";
	case WHILETK:
		return "WHILETK";
	case FORTK:
		return "FORTK";
	case SCANFTK:
		return "SCANFTK";
	case PRINTFTK:
		return "PRINTFTK";
	case RETURNTK:
		return "RETURNTK";
	case PLUS:
		return "PLUS";
	case MINU:
		return "MINU";
	case MULT:
		return "MULT";
	case DIV:
		return "DIV";
	case LSS:
		return "LSS";
	case LEQ:
		return "LEQ";
	case GRE:
		return "GRE";
	case GEQ:
		return "GEQ";
	case EQL:
		return "EQL";
	case NEQ:
		return "NEQ";
	case COLON:
		return "COLON";
	case ASSIGN:
		return "ASSIGN";
	case SEMICN:
		return "SEMICN";
	case COMMA:
		return "COMMA";
	case LPARENT:
		return "LPARENT";
	case RPARENT:
		return "RPARENT";
	case LBRACK:
		return "LBRACK";
	case RBRACK:
		return "RBRACK";
	case LBRACE:
		return "LBRACE";
	case RBRACE:
		return "RBRACE";
	default:
		return "******ERROR******";
	}
}

