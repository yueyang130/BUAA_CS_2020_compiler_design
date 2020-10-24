#include "GrammerAnalyzer.h"
#include "SymbolTable.h"
#include "error.h"
#include <algorithm>

GrammerAnalyzer::GrammerAnalyzer(LexicalAnalyzer& my_lexical_analyzer):
	sym_infor_list_(my_lexical_analyzer.sym_infor_list_), 
	error_infor_list_(my_lexical_analyzer.error_infor_list_),
	lexical_analyzer_(my_lexical_analyzer),
	sym_table_(SymbolTable::getInstance())
{
	tk_idx_ = 0;
	error_infor_list_.reserve(INIT_LENGTH / 4);
	output_list_.reserve(INIT_LENGTH);
}

GrammerAnalyzer& GrammerAnalyzer::getInstance(LexicalAnalyzer& my_lexical_analyzer) {
	static GrammerAnalyzer instance(my_lexical_analyzer);
	return instance;
}

void GrammerAnalyzer::analyzeGrammer() {
	Program();
}

/*输出词法分析的结果*/
void GrammerAnalyzer::show(ostream& fout) {
	for (auto& x : output_list_) {
		fout << x << endl;
	}
}

/*输出错误处理的结果*/
void GrammerAnalyzer::showError(ostream& fout) {
	sort(error_infor_list_.begin(), error_infor_list_.end());
	for (auto x : error_infor_list_) {
		fout << x.row << " " << char('a' + x.error_type) << endl;
	} 
}

/**
*										 错误检查与处理函数
*/

void GrammerAnalyzer::checkUndefine() {
	// 引用未定义的名字
	TableEntry* entry = sym_table_.find(curr_sym_str());
	if (!entry) {
		addErrorInfor(ErrorType::Undefine);
	}
}

/** 
*									语法分析程序
* 0. 如果有多个选择，则在上层对first进行检查；如果只有一个选择，则在本层再对first进行检查。
* 1.保证进入每个分程序时，sym_idx_指向分程序对应的第一个sym
* 2.在退出分程序前，将要输出的词法成分添加到output_list_
*  并将sym_idx指向分程序对应语法成分后的第一个sym
*/




/**
＜字符串＞   ::=  "｛十进制编码为32,33,35-126的ASCII字符｝" 

字符串中要求至少有一个字符
*/
void GrammerAnalyzer::String() {
	check(symbolType::STRCON);
	pop_sym();
	output_list_.push_back("<字符串>");
}

/**
* 1. ＜程序＞    ::= ［＜常量说明＞］［＜变量说明＞］{＜有返回值函数定义＞|＜无返回值函数定义＞}＜主函数＞ 
* 2. 通过预读判断是 <有返回值的函数声明> 还是  <变量声明>
* <变量声明> = （int|char) <标识符> ('['|,|;|=)
* <有返回值的函数定义> = (int|char) <标识符> '('
*/
void GrammerAnalyzer::Program() {
	if (sym_infor_list_.size() == 0) {
		return;
	}
	
	if (equal(symbolType::CONSTTK)) {
		ConstDeclare();
	}

	if (equal(INTTK, CHARTK)) {
		if (peek_sym_type(1) == symbolType::IDENFR) {
			symbolType peek2 = peek_sym_type(2);
			if (peek2 == symbolType::LBRACK || peek2 == symbolType::SEMICN ||
				peek2 == symbolType::COMMA || peek2 == symbolType::ASSIGN) {
				VarDeclare();
			}
		} 

	} 

	while (isFunctionNoReturn() || isFunctionWithReturn()) {
		if (isFunctionWithReturn()) {
			FuncDefineWithReturn(); 
		} else { FuncDefineNoReturn();  }
	}

	if (equal(VOIDTK) && peek_sym_type(1) == symbolType::MAINTK) {
		Main();
	} 
	output_list_.push_back("<程序>");
}

void GrammerAnalyzer::RepeatIfMark(void(GrammerAnalyzer::*handler)(symbolType), symbolType var_type, symbolType mark) {
	while (equal(mark)) {
		pop_sym();
		(this->*handler)(var_type);
	}
}


/**
* ＜常量说明＞ ::=  const＜常量定义＞;{ const＜常量定义＞;}
*/
void GrammerAnalyzer::ConstDeclare() {
	pop_sym();
	ConstDefine();
	check(symbolType::SEMICN);
	pop_sym();
	while (equal(symbolType::CONSTTK)) {
		pop_sym();
		ConstDefine();
		check(symbolType::SEMICN);
		pop_sym();
	}
	output_list_.push_back("<常量说明>");
}


/**
* ＜常量定义＞   ::=   int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞}  
                  | char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}  
*/


void GrammerAnalyzer::ConstDefine() {
	if (equal(symbolType::INTTK)) {
		pop_sym();
		ConstDefineForInt();
			
		while (equal(symbolType::COMMA)) {
			pop_sym();
			ConstDefineForInt();
		}

	} else if (equal(symbolType::CHARTK)) {  
		pop_sym();
		ConstDefineForChar();
		
		while (equal(symbolType::COMMA)) {
			pop_sym();
			ConstDefineForChar();
		}
	}

	output_list_.push_back("<常量定义>");

}

/**
* ＜标识符＞＝＜整数＞
*/
void GrammerAnalyzer::ConstDefineForInt() {
	check(symbolType::IDENFR);
	// 重名检查
	if (!sym_table_.add(ConstEntry(ValueType::INTV, curr_sym_str()))) {
		addErrorInfor(ErrorType::Redefine);
	}
	pop_sym();
	check(symbolType::ASSIGN);
	pop_sym();
	Int();
}

/**
* ＜标识符＞＝＜字符＞
*/
void GrammerAnalyzer::ConstDefineForChar() {
	check(symbolType::IDENFR);
	// 重名检查
	if (!sym_table_.add(ConstEntry(ValueType::CHARV, curr_sym_str()))) {
		addErrorInfor(ErrorType::Redefine);
	}
	pop_sym();
	check(symbolType::ASSIGN);
	pop_sym();
	check(symbolType::CHARCON);
	pop_sym();
}


/**
＜无符号整数＞  ::= ＜数字＞｛＜数字＞｝
*/
void GrammerAnalyzer::UnsignedInt() {
	check(INTCON);
	pop_sym();
	output_list_.push_back("<无符号整数>");
}

/**
* ＜整数＞        ::= ［＋｜－］＜无符号整数＞
*/
void GrammerAnalyzer::Int() {
	if (equal(symbolType::PLUS, symbolType::MINU)) {
		pop_sym();
	}
	UnsignedInt();
	output_list_.push_back("<整数>");
}


void GrammerAnalyzer::Identifier() {
	check(symbolType::IDENFR);
	pop_sym();
}

/**
＜常量＞   ::=  ＜整数＞|＜字符＞
*/
void GrammerAnalyzer::ConstValue(symbolType var_type) {
	if (var_type == symbolType::INTTK) {
		Int(); 
	} else if (var_type == symbolType::CHARTK) { 
		check(symbolType::CHARCON); 
		pop_sym();
	} else { error(); }

	output_list_.push_back("<常量>");
}

void GrammerAnalyzer::ConstValue() {
	if (equal(symbolType::CHARCON)) {
		pop_sym();
	} else {
		Int();
	}
	output_list_.push_back("<常量>");
}

/**
＜变量说明＞  ::= ＜变量定义＞;{＜变量定义＞;}
*/
void GrammerAnalyzer::VarDeclare() {
	VarDefine();
	check(symbolType::SEMICN);
	pop_sym();
	// 判断是<变量定义> 还是 <有返回值函数定义>
	while (true) {
		if (!equal(symbolType::INTTK, symbolType::CHARTK)) { break; }
		if (peek_sym_type(1) != symbolType::IDENFR) { break;  }
		symbolType peek2 = peek_sym_type(2);
		if (!(peek2 == symbolType::LBRACK || peek2 == symbolType::SEMICN ||
			peek2 == symbolType::COMMA || peek2 == symbolType::ASSIGN)) {
			break;
		}
		VarDefine();
		check(SEMICN);
		pop_sym();
	}
	output_list_.push_back("<变量说明>");
}

/**
* ＜变量定义＞ ::= ＜变量定义无初始化＞|＜变量定义及初始化＞
*/
void GrammerAnalyzer::VarDefine() {
	check(INTTK, CHARTK);

	// 通过在分号前是否出现等号判断是否初始化
	bool no_init = true;
	for (int i = 2; peek_sym_type(i) != symbolType::SEMICN; i++) {
		if (peek_sym_type(i) == symbolType::ASSIGN) {
			no_init = false;
		}
	}
	if (no_init) { VarDefineNoInit(); }
	else { VarDefineWithInit(); }
	

	output_list_.push_back("<变量定义>");
}

/**
* ＜变量定义无初始化＞  ::= ＜类型标识符＞(＜标识符＞
	|＜标识符＞'['＜无符号整数＞']'
	|＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']')
	{,(＜标识符＞|＜标识符＞'['＜无符号整数＞']'|＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']' )}
*/
void GrammerAnalyzer::VarDefineNoInit() {
	check(INTTK, CHARTK);
	ValueType value_type = (curr_sym_type() == INTTK) ? ValueType::INTV : ValueType::CHARV;
	pop_sym();
	VarDefineType(value_type);
	while (equal(COMMA)) {
		pop_sym();
		VarDefineType(value_type);
	}

	output_list_.push_back("<变量定义无初始化>");
}

/**
* (＜标识符＞|
  ＜标识符＞'['＜无符号整数＞']'|
  ＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']'
*/
void GrammerAnalyzer::VarDefineType(ValueType entry_value_type) {
	check(IDENFR);
	// 重名检查
	if (!sym_table_.add(VarEntry(entry_value_type, curr_sym_str()))) {
		addErrorInfor(ErrorType::Redefine);
	}
	pop_sym();
	if (equal(LBRACK)) {
		pop_sym();
		UnsignedInt();
		check(symbolType::RBRACK);
		pop_sym();
		if (equal(symbolType::LBRACK)) {
			pop_sym();
			UnsignedInt();
			check(symbolType::RBRACK);
			pop_sym();
		}
	}
}

/**
* ＜变量定义及初始化＞  ::= ＜类型标识符＞＜标识符＞=＜常量＞|
	＜类型标识符＞＜标识符＞'['＜无符号整数＞']'='{'＜常量＞{,＜常量＞}'}'|
	＜类型标识符＞＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']'='{''{'＜常量＞{,＜常量＞}'}'{, '{'＜常量＞{,＜常量＞}'}'}'}'

* 	有初始化的变量定义一次只能对一个标识符进行初始化
*/
void GrammerAnalyzer::VarDefineWithInit() {
	check(INTTK, CHARTK);
	symbolType var_type = curr_sym_type();
	ValueType value_type = (var_type == INTTK) ? ValueType::INTV : ValueType::CHARV;
	pop_sym();
	check(IDENFR);
	// 重名检查
	if (!sym_table_.add(VarEntry(value_type, curr_sym_str()))) {
		addErrorInfor(ErrorType::Redefine);
	}
	pop_sym();
	if (equal(symbolType::ASSIGN)) {   // ＜类型标识符＞＜标识符＞=＜常量＞
		pop_sym();
		ConstValue(var_type);

	} else if (equal(symbolType::LBRACK)) {
		pop_sym();
		UnsignedInt();
		check(RBRACK); pop_sym();
		
		if (equal(symbolType::ASSIGN)) {    // ＜类型标识符＞＜标识符＞'['＜无符号整数＞']'='{'＜常量＞{,＜常量＞}'}'
			check(ASSIGN); pop_sym();
			check(LBRACE); pop_sym();
			ConstValue(var_type);
			RepeatIfMark(&GrammerAnalyzer::ConstValue, var_type);
			check(RBRACE); pop_sym();

		} else if(equal(symbolType::LBRACK)) {  
			/*  ＜类型标识符＞＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']'='
			{''{'＜常量＞{,＜常量＞}'}'{, '{'＜常量＞{,＜常量＞}'}'   }'}' */
			pop_sym();
			UnsignedInt();
			check(RBRACK); pop_sym();
			check(ASSIGN); pop_sym();
			check(LBRACE); pop_sym();
			check(LBRACE); pop_sym();
			ConstValue(var_type);
			RepeatIfMark(&GrammerAnalyzer::ConstValue, var_type);
			check(RBRACE); pop_sym();

			while (equal(symbolType::COMMA)) {   // {,  '{'＜常量＞{,＜常量＞}'}'   }
				pop_sym();
				check(LBRACE); pop_sym();
				ConstValue(var_type);
				RepeatIfMark(&GrammerAnalyzer::ConstValue, var_type);
				check(RBRACE); pop_sym();
			}
			check(RBRACE); pop_sym();

		} else { error(); }

	} else { error(); }

	output_list_.push_back("<变量定义及初始化>");
}

/**
＜有返回值函数定义＞  ::=  ＜声明头部＞'('＜参数表＞')' '{'＜复合语句＞'}'

有返回值的函数的return语句错误：
	1.有返回值的函数无任何返回语句；
	2.或有形如return;的语句；
	3.或有形如return();的语句；
	4.或return语句中表达式类型与返回值类型不一致

其中只有1是到了函数结束才报错，报错时机是遇到'{'后,弹出'}'前；
2,3,4在遇到不匹配的return语句时就应该报错。
	
*/
void GrammerAnalyzer::FuncDefineWithReturn() {
	ValueType value_type;
	string func_name;
	vector<ValueType> formal_param_list;
	FuncDefineHead(func_name, value_type);
	check(symbolType::LPARENT);
	pop_sym();
	ParameterList(formal_param_list);
	// 将函数项添加到符号表中
	sym_table_.add(FunctionEntry(value_type, func_name, formal_param_list));

	check(symbolType::RPARENT);
	pop_sym();
	check(symbolType::LBRACE);
	pop_sym();
	bool exsit_return = false;
	CompoundStatement(&exsit_return, value_type);
	check(symbolType::RBRACE);
	/*
	有返回值的函数无任何返回语句；
	*/
	if (!exsit_return) {
		addErrorInfor(ErrorType::ReturnErrorInNonvoidFunc);
	}
	pop_sym();

	output_list_.push_back("<有返回值函数定义>");
}

/**
＜无返回值函数定义＞  ::= void＜标识符＞'('＜参数表＞')''{'＜复合语句＞'}'

出现了形如return(表达式);或return();的语句均报此错误
*/
void GrammerAnalyzer::FuncDefineNoReturn() {
	check(symbolType::VOIDTK);
	pop_sym();
	check(symbolType::IDENFR);
	// 重名检查
	string func_name = curr_sym_str();
	if (!sym_table_.checkRedefine(curr_sym_str())) {
		addErrorInfor(ErrorType::Redefine);
	}
	pop_sym();
	check(symbolType::LPARENT);
	pop_sym();
	vector<ValueType> formal_param_list;
	ParameterList(formal_param_list);
	// 将函数项添加到符号表中
	sym_table_.add(FunctionEntry(ValueType::VOIDV, func_name, formal_param_list));

	check(symbolType::RPARENT);
	pop_sym();
	check(symbolType::LBRACE);
	pop_sym();

	bool exsit_return;
	CompoundStatement(&exsit_return, ValueType::VOIDV);
	check(symbolType::RBRACE);
	pop_sym();
	output_list_.push_back("<无返回值函数定义>");
}

/**
*＜声明头部＞   ::=  int＜标识符＞ |char＜标识符＞ 
*/
void GrammerAnalyzer::FuncDefineHead(string& func_name, ValueType& value_type) {
	check(symbolType::INTTK, symbolType::CHARTK);
	value_type = (curr_sym_type() == INTTK) ? ValueType::INTV : ValueType::CHARV;
	pop_sym();
	check(IDENFR);
	func_name = curr_sym_str();
	// 重名检查
	if (!sym_table_.checkRedefine(curr_sym_str())) {
		addErrorInfor(ErrorType::Redefine);
	}
	pop_sym();
	output_list_.push_back("<声明头部>");
}

/**
＜参数表＞    ::=  ＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞}| ＜空＞
＜类型标识符＞      ::=  int | char  
*/
void GrammerAnalyzer::ParameterList(vector<ValueType>& formal_param_list) {
	if (equal(INTTK, CHARTK)) {
		ValueType value_type = (curr_sym_type() == INTTK) ? ValueType::INTV : ValueType::CHARV;
		formal_param_list.push_back(value_type);
		pop_sym();
		check(symbolType::IDENFR);
		pop_sym();
		while (equal(symbolType::COMMA)) {
			pop_sym();
			check(INTTK, CHARTK);
			ValueType value_type = (curr_sym_type() == INTTK) ? ValueType::INTV : ValueType::CHARV;
			formal_param_list.push_back(value_type);
			pop_sym();
			check(symbolType::IDENFR);
			pop_sym();
		}
	}
	output_list_.push_back("<参数表>");
}

/**
＜主函数＞    ::= void main‘(’‘)’ ‘{’＜复合语句＞‘}’
*/
void GrammerAnalyzer::Main() {
	check(symbolType::VOIDTK);
	pop_sym();
	check(symbolType::MAINTK);
	pop_sym();
	check(symbolType::LPARENT);
	pop_sym();
	check(symbolType::RPARENT);
	pop_sym();
	check(symbolType::LBRACE);
	pop_sym();
	bool exsit_return;
	CompoundStatement(&exsit_return ,ValueType::VOIDV);
	check(symbolType::RBRACE);
	pop_sym();
	output_list_.push_back("<主函数>");
}

/**
＜表达式＞    ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}
*/
void GrammerAnalyzer::Expr() {
	if (equal(symbolType::PLUS, symbolType::MINU)) {
		pop_sym();
		Item();
	} else {
		Item();
	}
	while (equal(symbolType::PLUS, symbolType::MINU)) {
		pop_sym();
		Item();
	}
	output_list_.push_back("<表达式>");
}

/**
＜项＞     ::= ＜因子＞{＜乘法运算符＞＜因子＞}
*/
void GrammerAnalyzer::Item() {
	Factor();
	while (equal(symbolType::MULT, symbolType::DIV)) {
		pop_sym();
		Factor();
	}
	output_list_.push_back("<项>");
}


/**
＜因子＞    ::= ＜标识符＞
				｜＜标识符＞'['＜表达式＞']'
				 |＜标识符＞'['＜表达式＞']''['＜表达式＞']'
				 |'('＜表达式＞')'
			     ｜＜整数＞
				 |＜字符＞
				｜＜有返回值函数调用语句＞

＜整数＞        ::= ［＋｜－］＜无符号整数＞
＜有返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')'

1.＜标识符＞'['＜表达式＞']'和＜标识符＞'['＜表达式＞']''['＜表达式＞']'中的＜表达式＞只能是整型，下标从0开始

2.单个＜标识符＞不包括数组名，即数组不能整体参加运算，数组元素可以参加运算
*/
void GrammerAnalyzer::Factor() {
	if (equal(symbolType::IDENFR)) {
		string identifier = curr_sym_str();
		checkUndefine();

		if (this->peek_sym_type()  == symbolType::LBRACK) {
			pop_sym();  // pop identifier
			pop_sym();  // pop '['
			Expr();
			check(symbolType::RBRACK);
			pop_sym();
			if (equal(symbolType::LBRACK)) {									// ＜标识符＞'['＜表达式＞']''['＜表达式＞']'
				pop_sym();
				Expr();
				check(symbolType::RBRACK);
				pop_sym();

			} else {															// ＜标识符＞'['＜表达式＞']'

			}

		} else if (this->peek_sym_type() == symbolType::LPARENT) {				// 有返回值函数调用
			TableEntry* p_entry = sym_table_.find(identifier);
			if (!p_entry || p_entry->entry_type() != EntryType::FUNCTION || p_entry->entry_value_type() == ValueType::VOIDV) {
				error();
			}
			CallWithReturn((FunctionEntry*)p_entry);
		} else {																// ＜标识符＞
			pop_sym();	 // pop identifier
		}

	} else if (equal(symbolType::LPARENT)) {									// '('＜表达式＞')'
		pop_sym();
		Expr();
		check(RPARENT);
		pop_sym();

	} else if (equal(symbolType::CHARCON)) {									// ＜字符＞
		pop_sym();
	} else if (equal(symbolType::PLUS, symbolType(MINU)) || equal(symbolType::INTCON)) {	// ＜整数＞
		Int();
	} else { error(); }
	output_list_.push_back("<因子>");
}

/**
＜复合语句＞   ::=  ［＜常量说明＞］［＜变量说明＞］＜语句列＞
*/
void GrammerAnalyzer::CompoundStatement(bool* p_exsit_return, ValueType return_value_type) {
	if (equal(symbolType::CONSTTK)) {
		ConstDeclare();
	}
	if (equal(symbolType::INTTK, symbolType::CHARTK)) {
		VarDeclare();
	}
	StatetmentList(p_exsit_return, return_value_type);
	output_list_.push_back("<复合语句>");

}

/**
＜语句列＞   ::= ｛＜语句＞｝
这里的{}指的是0到无穷个，不是'{'
*/
void GrammerAnalyzer::StatetmentList(bool* p_exsit_return, ValueType return_value_type) {
	while (!equal(symbolType::RBRACE)) {
		Statement(p_exsit_return, return_value_type);
	}
	output_list_.push_back("<语句列>");
}

/**
＜语句＞    ::= ＜循环语句＞｜＜条件语句＞|
			＜有返回值函数调用语句＞;  |＜无返回值函数调用语句＞;｜
			＜赋值语句＞;｜＜读语句＞;｜＜写语句＞;｜＜情况语句＞｜
			＜空＞;|＜返回语句＞; | '{'＜语句列＞'}'
*/
void GrammerAnalyzer::Statement(bool* p_exsit_return, ValueType return_value_type) {
	switch (curr_sym_type()) {
	case symbolType::WHILETK:
		LoopStatement(p_exsit_return, return_value_type);
		break;
	case symbolType::FORTK:
		LoopStatement(p_exsit_return, return_value_type);
		break;
	case symbolType::IFTK:
		ConditionalStatement(p_exsit_return, return_value_type);
		break;
	case symbolType::IDENFR:
		// 函数调用语句| 赋值语句
		if (peek_sym_type(1) == symbolType::ASSIGN || peek_sym_type() == symbolType::LBRACK) {
			AssignStatement();
			check(symbolType::SEMICN);
			pop_sym();
		} else if(peek_sym_type() == symbolType::LPARENT) {
			CallFunc();
			check(symbolType::SEMICN);
			pop_sym();
		}
		break;
	case symbolType::SCANFTK:
		ReadStatement();
		check(symbolType::SEMICN);
		pop_sym();
		break;
	case symbolType::PRINTFTK:
		WriteStatement();
		check(symbolType::SEMICN);
		pop_sym();
		break;
	case symbolType::SWITCHTK:
		SwitchStatement(p_exsit_return, return_value_type);
		break;
	case symbolType::SEMICN:
		pop_sym();
		break;
	case symbolType::RETURNTK:
		ReturnStatement(p_exsit_return, return_value_type);
		check(symbolType::SEMICN);
		pop_sym();
		break;
	case symbolType::LBRACE:
		pop_sym();
		StatetmentList(p_exsit_return, return_value_type);
		check(symbolType::RBRACE);
		pop_sym();
		break;
	default:
		error();
		break;
	}
	output_list_.push_back("<语句>");
}

/**
＜赋值语句＞   ::=  ＜标识符＞＝＜表达式＞
		|＜标识符＞'['＜表达式＞']'=＜表达式＞
		|＜标识符＞'['＜表达式＞']''['＜表达式＞']' =＜表达式＞

＜标识符＞＝＜表达式＞中的<标识符>不能为常量名和数组名
*/
void GrammerAnalyzer::AssignStatement() {
	check(symbolType::IDENFR);
	checkUndefine();
	pop_sym();
	if (equal(symbolType::LBRACK)) {
		pop_sym();
		Expr();
		check(symbolType::RBRACK);
		pop_sym();
		if (equal(symbolType::LBRACK)) {
			pop_sym();
			Expr();
			check(symbolType::RBRACK);
			pop_sym();
		}
	}
	check(symbolType::ASSIGN);
	pop_sym();
	Expr();
	output_list_.push_back("<赋值语句>");
}

/**
＜条件语句＞  ::= if '('＜条件＞')'＜语句＞［else＜语句＞］
*/
void GrammerAnalyzer::ConditionalStatement(bool* p_exsit_return, ValueType return_value_type) {
	check(symbolType::IFTK);
	pop_sym();
	check(symbolType::LPARENT);
	pop_sym();
	Condition();
	check(symbolType::RPARENT);
	pop_sym();
	Statement(p_exsit_return, return_value_type);
	if (equal(symbolType::ELSETK)) {
		pop_sym();
		Statement(p_exsit_return, return_value_type);
	}
	output_list_.push_back("<条件语句>");
}

/**
＜条件＞    ::=  ＜表达式＞＜关系运算符＞＜表达式＞
＜关系运算符＞  ::=  <｜<=｜>｜>=｜!=｜==
表达式需均为整数类型才能进行比较
*/
void GrammerAnalyzer::Condition() {
	// TODO: 表达式需均为整数类型才能进行比较
	ValueType value_type1 = Expr();
	if (equal(LSS, LEQ) || equal(GRE, GEQ) || equal(NEQ, EQL)) {
		pop_sym();
	}else { error(); }
	ValueType value_type2 = Expr();
	if (value_type1 != ValueType::INTV || value_type2 != ValueType::INTV) {
		addErrorInfor(ErrorType::IllegalCondition);
	}
	output_list_.push_back("<条件>");
}

/**
＜循环语句＞   ::=  while '('＜条件＞')'＜语句＞
	| for'('＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞')'＜语句＞
*/
void GrammerAnalyzer::LoopStatement(bool* p_exsit_return, ValueType return_value_type) {
	if (equal(WHILETK)) {
		pop_sym();
		check(LPARENT);
		pop_sym();
		Condition();
		check(RPARENT);
		pop_sym();
		Statement(p_exsit_return, return_value_type);

	} else if (equal(FORTK)) {
		// for'('＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞')'＜语句＞
		pop_sym();
		check(LPARENT);
		pop_sym();
		check(IDENFR);
		checkUndefine();
		pop_sym();
		check(ASSIGN);
		pop_sym();
		Expr();
		check(SEMICN);
		pop_sym();
		Condition();
		check(SEMICN);
		pop_sym();
		check(IDENFR);
		checkUndefine();
		pop_sym();
		check(ASSIGN); pop_sym();
		check(IDENFR); 
		checkUndefine();
		pop_sym();
		check(PLUS, MINU);
		pop_sym();
		Step();
		check(RPARENT);
		pop_sym();
		Statement(p_exsit_return, return_value_type);

	} else { error(); }
	
	output_list_.push_back("<循环语句>");
}

/**
＜步长＞::= ＜无符号整数＞
*/
void GrammerAnalyzer::Step() {
	UnsignedInt();
	output_list_.push_back("<步长>");
}

/**
＜情况语句＞  ::=  switch ‘(’＜表达式＞‘)’ ‘{’＜情况表＞＜缺省＞‘}’

情况语句中，switch后面的表达式和case后面的常量只允许出现int和char类型；
每个情况子语句执行完毕后，不继续执行后面的情况子语句
*/
void GrammerAnalyzer::SwitchStatement(bool* p_exsit_return, ValueType return_value_type) {
	check(symbolType::SWITCHTK);
	pop_sym();
	check(symbolType::LPARENT);
	pop_sym();
	Expr();
	check(symbolType::RPARENT);
	pop_sym();
	check(symbolType::LBRACE);
	pop_sym();
	CaseList(p_exsit_return, return_value_type);
	SwitchDefault(p_exsit_return, return_value_type);
	check(symbolType::RBRACE);
	pop_sym();
	output_list_.push_back("<情况语句>");
}

/**
＜情况表＞   ::=  ＜情况子语句＞{＜情况子语句＞}
*/
void GrammerAnalyzer::CaseList(bool* p_exsit_return, ValueType return_value_type) {
	CaseStatement(p_exsit_return, return_value_type);
	while (equal(symbolType::CASETK)) {
		CaseStatement(p_exsit_return, return_value_type);
	}
	output_list_.push_back("<情况表>");
}

/**
＜情况子语句＞  ::=  case＜常量＞：＜语句＞
＜常量＞   ::=  ＜整数＞|＜字符＞
*/
void GrammerAnalyzer::CaseStatement(bool* p_exsit_return, ValueType return_value_type) {
	// TODO: 检查常量与switch类型是否匹配
	check(symbolType::CASETK);
	pop_sym();
	ConstValue();
	check(symbolType::COLON);
	pop_sym();
	Statement(p_exsit_return, return_value_type);
	output_list_.push_back("<情况子语句>");
}

/**
＜缺省＞   ::=  default :＜语句＞
*/
void GrammerAnalyzer::SwitchDefault(bool* p_exsit_return, ValueType return_value_type) {
	check(symbolType::DEFAULTTK);
	pop_sym();
	check(symbolType::COLON);
	pop_sym();
	Statement(p_exsit_return, return_value_type);
	output_list_.push_back("<缺省>");
}

void GrammerAnalyzer::CallFunc() {
	check(symbolType::IDENFR);
	string& func_name = curr_sym_str();
	TableEntry* p_entry = sym_table_.find(func_name);
	// 按照助教的解释，不会出现未定义的函数调用。避免了未知返回值类型、参数表等多项可能会产生歧义的错误。
	// 但是还是进行一下函数名是否定义的检查。
	if (!p_entry || p_entry->entry_type() != EntryType::FUNCTION) {
		error();
	}
	if (p_entry->entry_value_type() == ValueType::VOIDV) {
		CallNoReturn((FunctionEntry*) p_entry);
	} else {
		CallWithReturn((FunctionEntry*) p_entry);
	}

}

/**
＜有返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')'
函数调用时，只能调用在之前已经定义过的函数，对是否有返回值的函数都是如此
*/
void GrammerAnalyzer::CallWithReturn(FunctionEntry* p_entry) {
	check(symbolType::IDENFR);

	pop_sym();
	check(symbolType::LPARENT);
	pop_sym();
	ValueParameterList(p_entry);
	check(RPARENT);
	pop_sym();
	output_list_.push_back("<有返回值函数调用语句>");
}

/**
＜无返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')'
*/
void GrammerAnalyzer::CallNoReturn(FunctionEntry* p_entry) {
	check(symbolType::IDENFR);
	pop_sym();
	check(symbolType::LPARENT);
	pop_sym();
	ValueParameterList(p_entry);
	check(RPARENT);
	pop_sym();
	output_list_.push_back("<无返回值函数调用语句>");
}

/**
＜值参数表＞   ::= ＜表达式＞{,＜表达式＞}｜＜空＞

实参的表达式不能是数组名，可以是数组元素
实参的计算顺序,要求生成的目标码运行结果与Clang8.0.1 编译器运行的结果一致
*/
void GrammerAnalyzer::ValueParameterList(FunctionEntry* p_entry) {
	vector<ValueType> actual_param_list;
	if (!equal(symbolType::RPARENT)) {
		ValueType value_type;
		value_type = Expr();
		actual_param_list.push_back(value_type);
		while (equal(symbolType::COMMA)) {
			pop_sym();
			value_type = Expr();
			actual_param_list.push_back(value_type);
		}
	}
	// 一行最多出现一个错误，因此使用if - else if的逻辑
	if (p_entry->formal_param_num() != actual_param_list.size()) {
		addErrorInfor(ErrorType::FuncParamNumUnmatch);
	} else if (p_entry->checkParamList(actual_param_list)) {
		addErrorInfor(ErrorType::FuncParamTypeUnmatch);
	}

	output_list_.push_back("<值参数表>");
}

/**
＜读语句＞    ::=  scanf '('＜标识符＞')'

从标准输入获取<标识符>的值，该标识符不能是常量名和数组名
生成的PCODE或MIPS汇编在运行时，对每一个scanf语句，无论标识符的类型是char还是int，末尾均需回车；在testin.txt文件中的输入数据也是每项在一行
生成MIPS汇编时按照syscall指令的用法使用
*/
void GrammerAnalyzer::ReadStatement() {
	check(symbolType::SCANFTK);
	pop_sym();
	check(symbolType::LPARENT);
	pop_sym();
	check(symbolType::IDENFR);
	checkUndefine();
	pop_sym();
	check(symbolType::RPARENT);
	pop_sym();

	output_list_.push_back("<读语句>");
}

/**
＜写语句＞    ::= printf '(' ＜字符串＞,＜表达式＞ ')'| printf '('＜字符串＞ ')'| printf '('＜表达式＞')'

printf '(' ＜字符串＞,＜表达式＞ ')'输出时，先输出字符串的内容，再输出表达式的值，两者之间无空格
表达式为字符型时，输出字符；为整型时输出整数
＜字符串＞原样输出（不存在转义）
每个printf语句的内容输出到一行，按结尾有换行符\n处理
*/
void GrammerAnalyzer::WriteStatement() {
	check(symbolType::PRINTFTK);
	pop_sym();
	check(symbolType::LPARENT);
	pop_sym();
	if (equal(symbolType::STRCON)) {
		String();
		if (equal(symbolType::COMMA)) {
			pop_sym();
			Expr();
		}
	} else {
		Expr();
	}
	check(symbolType::RPARENT);
	pop_sym();
	
	output_list_.push_back("<写语句>");
}

/**
＜返回语句＞   ::=  return['('＜表达式＞')']  这里的()属于return语句，不属于表达式

无返回值的函数中可以没有return语句，也可以有形如return;的语句，
无返回值的函数中若出现了形如return(表达式);或return();的语句均报此错误

有返回值的函数只要出现一条带返回值的return语句（表达式带小括号）即可.
有返回值的函数有形如return;的语句；
或有形如return();的语句；
或return语句中表达式类型与返回值类型不一致
*/
void GrammerAnalyzer::ReturnStatement(bool* p_exsit_return, ValueType return_value_type) {
	// 检查return语句与函数返回值类型的匹配
	*p_exsit_return = true;
	check(symbolType::RETURNTK);
	pop_sym();
	if (equal(symbolType::LPARENT)) {
		if (return_value_type == ValueType::VOIDV) {   // 无返回值的函数中若出现了形如return(表达式);或return();的语句
			addErrorInfor(ErrorType::ReturnErrorInVoidFunc);
		}
		pop_sym();
		if (!equal(symbolType::RPARENT)) {
			ValueType expr_value_type = Expr();
			if (expr_value_type != return_value_type)  {   // return语句中表达式类型与返回值类型不一致
				addErrorInfor(ErrorType::ReturnErrorInVoidFunc);
			}
		} else {   // 或有形如return();的语句；
			addErrorInfor(ErrorType::ReturnErrorInNonvoidFunc);
		}
		check(symbolType::RPARENT);
		pop_sym();
	} else if (return_value_type != ValueType::VOIDV) {  // 有返回值的函数有形如return;的语句；
		addErrorInfor(ErrorType::ReturnErrorInNonvoidFunc);
	}

	output_list_.push_back("<返回语句>");
}
