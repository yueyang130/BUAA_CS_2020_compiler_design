#include "GrammerAnalyzer.h"
#include "SymbolTable.h"
#include "error.h"
#include <algorithm>

GrammerAnalyzer::GrammerAnalyzer(LexicalAnalyzer& my_lexical_analyzer, IMCode& im_code):
	sym_infor_list_(my_lexical_analyzer.sym_infor_list_), 
	error_infor_list_(my_lexical_analyzer.error_infor_list_),
	lexical_analyzer_(my_lexical_analyzer),
	im_coder_(im_code),
	sym_table_(SymbolTable::getInstance())
{
	tk_idx_ = 0;
	error_infor_list_.reserve(INIT_LENGTH / 4);
	output_list_.reserve(INIT_LENGTH);
}

GrammerAnalyzer& GrammerAnalyzer::getInstance(LexicalAnalyzer& my_lexical_analyzer, IMCode& im_code) {
	static GrammerAnalyzer instance(my_lexical_analyzer, im_code);
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

shared_ptr<TableEntry> GrammerAnalyzer::checkUndefine() {
	// 引用未定义的名字
	shared_ptr<TableEntry> entry = sym_table_.checkReference(curr_sym_str());
	if (!entry) {
		addErrorInfor(ErrorType::Undefine);
	}
	return entry;
}

void GrammerAnalyzer::checkMissSemi() {
	if (!equal(symbolType::SEMICN)) { addErrorInfor(ErrorType::MissSemi, peek_sym_row(-1)); }
	else { pop_sym(); }
}

void GrammerAnalyzer::checkMissRparent() {
	if (!equal(symbolType::RPARENT)) { addErrorInfor(ErrorType::MissRparent); }
	else { pop_sym(); }
}

void GrammerAnalyzer::checkMissRbrack() {
	if (!equal(symbolType::RBRACK)) { addErrorInfor(ErrorType::MissRbrack);  }
	else { pop_sym(); }
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

int GrammerAnalyzer::RepeatIfMark(string (GrammerAnalyzer::*handler)(ValueType), ValueType var_type, symbolType mark, vector<string>* ptr_elems) {
	int cnt = 0;
	while (equal(mark)) {
		pop_sym();
		string str = (this->*handler)(var_type);
		if (ptr_elems) {
			ptr_elems->push_back(str);
		}
		cnt++;
	}
	return cnt;
}


/**
* ＜常量说明＞ ::=  const＜常量定义＞;{ const＜常量定义＞;}
*/
void GrammerAnalyzer::ConstDeclare() {
	pop_sym();
	ConstDefine();
	checkMissSemi();
	while (equal(symbolType::CONSTTK)) {
		pop_sym();
		ConstDefine();
		checkMissSemi();
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
	auto p_entry = make_shared<ConstEntry>(ValueType::INTV, curr_sym_str());
	if (!sym_table_.add(p_entry)) {
		addErrorInfor(ErrorType::Redefine);
	}
	pop_sym();
	check(symbolType::ASSIGN);
	pop_sym();
	int value = Int();
	p_entry->setValue(to_string(value));

	// 四元式生成
	im_coder_.addQuater(QuaternionFactory::ConstDeclare(p_entry));
}

/**
* ＜标识符＞＝＜字符＞
*/
void GrammerAnalyzer::ConstDefineForChar() {
	check(symbolType::IDENFR);
	// 重名检查
	auto p_entry = make_shared<ConstEntry>(ValueType::CHARV, curr_sym_str());
	if (!sym_table_.add(p_entry)) {
		addErrorInfor(ErrorType::Redefine);
	}
	pop_sym();
	check(symbolType::ASSIGN);
	pop_sym();
	check(symbolType::CHARCON);
	p_entry->setValue(curr_sym_str());
	pop_sym();

	//四元式生成
	im_coder_.addQuater(QuaternionFactory::ConstDeclare(p_entry));
}


/**
＜无符号整数＞  ::= ＜数字＞｛＜数字＞｝
*/
int GrammerAnalyzer::UnsignedInt() {
	check(INTCON);
	int value = std::stoi(curr_sym_str());
	pop_sym();
	output_list_.push_back("<无符号整数>");
	return value;
}


/**
* ＜整数＞        ::= ［＋｜－］＜无符号整数＞
*/
int GrammerAnalyzer::Int() {
	int value = 1;
	if (equal(symbolType::PLUS, symbolType::MINU)) {
		if (equal(symbolType::MINU)) { value = -1; }
		pop_sym();
	}
	value = value * UnsignedInt();
	output_list_.push_back("<整数>");
	return value;
}


void GrammerAnalyzer::Identifier() {
	check(symbolType::IDENFR);
	pop_sym();
}

/**
＜常量＞   ::=  ＜整数＞|＜字符＞
*/
string GrammerAnalyzer::ConstValue(ValueType left_value_type) {
	ValueType right_value_type;
	string const_value;
	if (equal(symbolType::CHARCON)) {
		const_value = curr_sym_str();
		pop_sym();
		right_value_type = ValueType::CHARV;
	}
	else {
		int int_value = Int();
		const_value = to_string(int_value);
		right_value_type = ValueType::INTV;
	}
	/*
	cout << "In line " << curr_sym_row() << endl;
	cout << "left value is " << left_value_type << ", right value is " << right_value_type << endl;
	*/

	if (left_value_type != right_value_type) {
		addErrorInfor(ErrorType::UnmatchedConstType);
	}

	output_list_.push_back("<常量>");
	return const_value;
}

string GrammerAnalyzer::ConstValue() {
	string const_value;
	if (equal(symbolType::CHARCON)) {
		const_value = curr_sym_str();
		pop_sym();
	} else {
		int int_value = Int();
		const_value = to_string(int_value);
	}
	output_list_.push_back("<常量>");
	return const_value;
}

/**
＜变量说明＞  ::= ＜变量定义＞;{＜变量定义＞;}
*/
void GrammerAnalyzer::VarDeclare() {
	VarDefine();
	checkMissSemi();
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
		checkMissSemi();
	}
	output_list_.push_back("<变量说明>");
}

/**
* ＜变量定义＞ ::= ＜变量定义无初始化＞|＜变量定义及初始化＞
* 
* version1：通过在分号前是否出现等号判断是否初始化，如果这一行缺失分号，可能导致意想不到的结果。
* version2：考虑到错误处理中同样会出现却少 ]等情况，所以决定以 int|char|void的最近的一个为结束符（其实就是换行）
*   
*/
void GrammerAnalyzer::VarDefine() {
	check(INTTK, CHARTK);

	
	bool no_init = true;

	for (int i = 2; peek_sym_type(i) != symbolType::INTTK && peek_sym_type(i) != CHARTK
		&& peek_sym_type(i) != VOIDTK && peek_sym_type(i) != SEMICN; i++) {
		if (peek_sym_type(i) == symbolType::ASSIGN) {
			no_init = false;
			break;
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
	string& var_name = curr_sym_str();
	vector<int> shape;
	if (!sym_table_.checkDefine(var_name)) {
		addErrorInfor(ErrorType::Redefine);
	}
	pop_sym();
	if (equal(LBRACK)) {
		pop_sym();
		shape.push_back(UnsignedInt());
		checkMissRbrack();
		if (equal(symbolType::LBRACK)) {
			pop_sym();
			shape.push_back(UnsignedInt());
			checkMissRbrack();
		}
	}
	auto p_entry = make_shared<VarEntry>(entry_value_type, var_name, shape);
	sym_table_.add(p_entry);
	// 添加四元式
	im_coder_.addQuater(QuaternionFactory::VarDecalre(p_entry));
}

/**
* ＜变量定义及初始化＞  ::= ＜类型标识符＞＜标识符＞=＜常量＞|
	＜类型标识符＞＜标识符＞'['＜无符号整数＞']'='{'＜常量＞{,＜常量＞}'}'|
	＜类型标识符＞＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']'='{''{'＜常量＞{,＜常量＞}'}'{, '{'＜常量＞{,＜常量＞}'}'}'}'

* 	有初始化的变量定义一次只能对一个标识符进行初始化
*/
void GrammerAnalyzer::VarDefineWithInit() {
	check(INTTK, CHARTK);
	vector<int> declared_shape;   // 左边定义的shape
	string e;					// 记录用于初始化的当前元素
	vector<string> elems;		// 记录用于初始化的元素列表
	//vector<int> initialized_shape;
	symbolType var_type = curr_sym_type();
	ValueType value_type = (var_type == INTTK) ? ValueType::INTV : ValueType::CHARV;
	pop_sym();
	check(IDENFR);
	string& var_name = curr_sym_str();
	// 重名检查
	if (!sym_table_.checkDefine(var_name)) {
		addErrorInfor(ErrorType::Redefine);
	}
	pop_sym();

	if (equal(symbolType::ASSIGN)) {   // ＜类型标识符＞＜标识符＞=＜常量＞
		pop_sym();
		e = ConstValue(value_type);
		elems.push_back(e);

	} else if (equal(symbolType::LBRACK)) {
		pop_sym();
		declared_shape.push_back(UnsignedInt());
		checkMissRbrack();
		
		if (equal(symbolType::ASSIGN)) {    // ＜类型标识符＞＜标识符＞'['＜无符号整数＞']'='{'＜常量＞{,＜常量＞}'}'
			check(ASSIGN); pop_sym();
			check(LBRACE); pop_sym();
			e = ConstValue(value_type);
			elems.push_back(e);
			int cnt = 1 + RepeatIfMark(&GrammerAnalyzer::ConstValue, value_type, COMMA, &elems);
			// check shape
			if (declared_shape[0] != cnt) { addErrorInfor(ErrorType::IllegalArrayInit); }
			check(RBRACE); pop_sym();

		} else if(equal(symbolType::LBRACK)) {  
			/*  ＜类型标识符＞＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']'='
			{''{'＜常量＞{,＜常量＞}'}'{, '{'＜常量＞{,＜常量＞}'}'   }'}' */
			int cnt1, cnt2;  // cnt1是第一维的大小，cnt2是第二维的大小

			pop_sym();
			declared_shape.push_back(UnsignedInt());
			checkMissRbrack();
			check(ASSIGN); pop_sym();
			check(LBRACE); pop_sym();
			check(LBRACE); pop_sym();
			e = ConstValue(value_type);
			elems.push_back(e);
			cnt2 = 1 + RepeatIfMark(&GrammerAnalyzer::ConstValue, value_type, COMMA, &elems);
			// check shape
			if (declared_shape[1] != cnt2) { addErrorInfor(ErrorType::IllegalArrayInit); }
			check(RBRACE); pop_sym();

			cnt1 = 1;
			while (equal(symbolType::COMMA)) {   // {,  '{'＜常量＞{,＜常量＞}'}'   }
				pop_sym();
				check(LBRACE); pop_sym();
				e = ConstValue(value_type);
				elems.push_back(e);
				cnt2 = 1 + RepeatIfMark(&GrammerAnalyzer::ConstValue, value_type, COMMA, &elems);
				if (declared_shape[1] != cnt2) { addErrorInfor(ErrorType::IllegalArrayInit); }
				check(RBRACE); pop_sym();
				cnt1++;
			}
			check(RBRACE); 
			// check shape
			if (declared_shape[0] != cnt1) { addErrorInfor(ErrorType::IllegalArrayInit); }
			pop_sym();
			  
		} else { error(); }

	} else { error(); }

	auto var_entry = make_shared<VarEntry>(value_type, var_name, declared_shape);
	sym_table_.add(var_entry);
	// 四元式生成
	auto immediate_entry = make_shared<ImmediateEntry>(value_type, declared_shape, elems);
	im_coder_.addQuater(QuaternionFactory::VarDecalre(var_entry, immediate_entry));

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

	// 将函数项添加到符号表中
	auto p_entry = make_shared<FunctionEntry>(value_type, func_name);
	sym_table_.add(p_entry);
	
	// 将函数项添加到符号表中后才能进行定位操作
	sym_table_.set();

	// 添加四元式
	im_coder_.addFunc(func_name);
	auto bblock = make_shared<BasicBlock>();
	im_coder_.addBBlock(bblock);
	im_coder_.addQuater(QuaternionFactory::FuncDeclareHead(p_entry));


	// 将形参加入符号表
	check(symbolType::LPARENT);
	pop_sym();
	ParameterList(formal_param_list);
	checkMissRparent();

	// 为FunctionEntry设置参数表信息
	p_entry->setParamList(formal_param_list);

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

	// 重定位操作
	sym_table_.reset();

	output_list_.push_back("<有返回值函数定义>");
}

/**
＜无返回值函数定义＞  ::= void＜标识符＞'('＜参数表＞')''{'＜复合语句＞'}'

出现了形如return(表达式);或return();的语句均报此错误
*/
void GrammerAnalyzer::FuncDefineNoReturn() {
	vector<ValueType> formal_param_list;


	check(symbolType::VOIDTK);
	pop_sym();
	check(symbolType::IDENFR);
	// 重名检查
	string func_name = curr_sym_str();
	if (!sym_table_.checkDefine(curr_sym_str())) {
		addErrorInfor(ErrorType::Redefine);
	}
	pop_sym();
	
	// 将函数项添加到符号表中
	auto p_entry = make_shared<FunctionEntry>(ValueType::VOIDV, func_name);
	sym_table_.add(p_entry);

	// 将函数项添加到符号表中后才能进行定位操作
	sym_table_.set();

	// 添加四元式
	im_coder_.addFunc(func_name);
	auto bblock = make_shared<BasicBlock>();
	im_coder_.addBBlock(bblock);
	im_coder_.addQuater(QuaternionFactory::FuncDeclareHead(p_entry));

	// 将形参加入符号表
	check(symbolType::LPARENT);
	pop_sym();
	ParameterList(formal_param_list);
	checkMissRparent();

	// 为FunctionEntry设置参数表信息
	p_entry->setParamList(formal_param_list);

	check(symbolType::LBRACE);
	pop_sym();

	bool exsit_return;
	CompoundStatement(&exsit_return, ValueType::VOIDV);
	check(symbolType::RBRACE);
	pop_sym();

	/* 
	* *无论是否存在返回语句，对于无返回值的函数，都应该在最后面加一个return
	// 如果不存在返回语句，生成返回语句的中间代码
	if (!exsit_return) {
		im_coder_.addQuater(QuaternionFactory::FuncReturn(nullptr));
	}
	*/
	im_coder_.addQuater(QuaternionFactory::FuncReturn(nullptr));

	// 重定位操作
	sym_table_.reset();

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
	if (!sym_table_.checkDefine(curr_sym_str())) {
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
	// 因为参数表可以为空，这个地方必须用if inttk or chartk,而不能用check(inttk, chartk)
	if (equal(INTTK, CHARTK)) {
		ValueType value_type = (curr_sym_type() == INTTK) ? ValueType::INTV : ValueType::CHARV;
		formal_param_list.push_back(value_type);
		pop_sym();
		check(symbolType::IDENFR);
		// 将形参加入符号表
		auto var_entry = make_shared<VarEntry>(value_type, curr_sym_str());
		sym_table_.add(var_entry);
		// 生成四元式
		im_coder_.addQuater(QuaternionFactory::FuncFormalParam(var_entry));
		pop_sym();
		while (equal(symbolType::COMMA)) {
			pop_sym();
			check(INTTK, CHARTK);
			ValueType value_type = (curr_sym_type() == INTTK) ? ValueType::INTV : ValueType::CHARV;
			formal_param_list.push_back(value_type);
			pop_sym();
			check(symbolType::IDENFR);
			// 将形参加入符号表
			auto var_entry = make_shared<VarEntry>(value_type, curr_sym_str());
			sym_table_.add(var_entry);
			// 生成四元式
			im_coder_.addQuater(QuaternionFactory::FuncFormalParam(var_entry));
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
	string func_name = curr_sym_str();
	pop_sym();
	check(symbolType::LPARENT);
	pop_sym();
	checkMissRparent();

	// 将函数项添加到符号表中
	auto p_entry = make_shared<FunctionEntry>(ValueType::VOIDV, func_name);
	sym_table_.add(p_entry);

	// 将函数项添加到符号表中后才能进行定位操作
	sym_table_.set();

	// 生成四元式
	im_coder_.addFunc(func_name);
	auto bblock = make_shared<BasicBlock>();
	im_coder_.addBBlock(bblock);
	bblock->addQuater(QuaternionFactory::FuncDeclareHead(p_entry));

	check(symbolType::LBRACE);
	pop_sym();
	bool exsit_return;
	CompoundStatement(&exsit_return ,ValueType::VOIDV);

	//// 如果不存在返回语句，生成返回语句的中间代码
	//if (!exsit_return) {
	//	im_coder_.addQuater(QuaternionFactory::FuncReturn(nullptr));
	//}
	im_coder_.addQuater(QuaternionFactory::FuncReturn(nullptr));

	check(symbolType::RBRACE);
	pop_sym();


	// 重定位操作
	sym_table_.reset();

	output_list_.push_back("<主函数>");
}

/**
＜表达式＞    ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}


表达式类型为char型有以下三种情况：

		 1）表达式由<标识符>、＜标识符＞'['＜表达式＞']和＜标识符＞'['＜表达式＞']''['＜表达式＞']'构成，
			且<标识符>的类型为char，即char类型的常量和变量、char类型的一维、二维数组元素。
		 2）表达式仅由一个<字符>构成，即字符字面量。
		 3）表达式仅由一个有返回值的函数调用构成，且该被调用的函数返回值为char型
*/
ValueType GrammerAnalyzer::Expr() {
	symbolType alu_type;
	bool exist_cal = false; // 存在加减运算的flag
	if (equal(symbolType::PLUS, symbolType::MINU)) {
		exist_cal = true;
		// 如果表达式开头是+/-,要补0（null)
		stack_push(nullptr);
		alu_type = curr_sym_type();
		pop_sym();
		
	}
	ValueType item1_value_type = Item();
	if (exist_cal) { 
		auto quater = stack_alu(alu_type);
		im_coder_.addQuater(quater);
	}

	while (equal(symbolType::PLUS, symbolType::MINU)) {
		exist_cal = true;
		alu_type = curr_sym_type();
		pop_sym();
		Item();
		auto quater = stack_alu(alu_type);
		im_coder_.addQuater(quater);
	}
	output_list_.push_back("<表达式>");

	if (exist_cal) {
		return ValueType::INTV;
	} else {
		return item1_value_type;
	}
}

/**
＜项＞     ::= ＜因子＞{＜乘法运算符＞＜因子＞}
*/
ValueType GrammerAnalyzer::Item() {
	ValueType item_value_type;
	item_value_type = Factor();
	while (equal(symbolType::MULT, symbolType::DIV)) {
		// 由+-运算，则一定转化为INT
		item_value_type = ValueType::INTV;

		auto alu_type = curr_sym_type();
		pop_sym();
		Factor();
		auto quater = stack_alu(alu_type);
		im_coder_.addQuater(quater);
	}
	output_list_.push_back("<项>");
	return item_value_type;
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
ValueType GrammerAnalyzer::Factor() {
	ValueType factor_value_type = ValueType::INTV;   // 默认为INT，除非碰到那三种情况才赋值为CHAR
	bool isArray = false;
	vector<shared_ptr<TableEntry>> indexs;

	if (equal(symbolType::IDENFR)) {
		string identifier = curr_sym_str();
		auto p_entry = checkUndefine();
	
		if (p_entry) { // 如果标识符有定义
			factor_value_type = p_entry->value_type();
		} else {
			factor_value_type = ValueType::UNKNOWN;
		}

		if (this->peek_sym_type() == symbolType::LBRACK) {
			isArray = true;
			pop_sym();  // pop identifier
			pop_sym();  // pop '['
			if (Expr() != ValueType::INTV) { addErrorInfor(ErrorType::IllegalArrayIndexType); }
			indexs.push_back(stack_pop_value());
			checkMissRbrack();
			if (equal(symbolType::LBRACK)) {									// ＜标识符＞'['＜表达式＞']''['＜表达式＞']'
				pop_sym();
				if (Expr() != ValueType::INTV) { addErrorInfor(ErrorType::IllegalArrayIndexType); }
				indexs.push_back(stack_pop_value());
				checkMissRbrack();
			} else {															// ＜标识符＞'['＜表达式＞']'

			}
			auto temp = make_shared<TempEntry>(this->new_temp(), factor_value_type);
			// 将pushIndex语句紧邻getArray和setArray语句
			for (auto x : indexs) {
				im_coder_.addQuater(QuaternionFactory::pushArrayIndex(x));
			}
			im_coder_.addQuater(QuaternionFactory::getArrayElem(temp, p_entry));
			this->stack_push(temp);

		} else if (this->peek_sym_type() == symbolType::LPARENT) {				// 有返回值函数调用
			shared_ptr<TableEntry> p_entry = sym_table_.checkReference(identifier);
			if (!p_entry || p_entry->entry_type() != EntryType::FUNCTION || p_entry->value_type() == ValueType::VOIDV) {
				error();
			}
			shared_ptr<TempEntry> ret;
			CallWithReturn(dynamic_pointer_cast<FunctionEntry>(p_entry), &ret);
			stack_push(ret);
		} else {																// ＜标识符＞
			stack_push(p_entry);
			pop_sym();	 // pop identifier
		}
	} else if (equal(symbolType::LPARENT)) {									// '('＜表达式＞')'
		pop_sym();
		Expr();
		checkMissRparent();

	} else if (equal(symbolType::CHARCON)) {									// ＜字符＞
		stack_push(make_shared<ImmediateEntry>(ValueType::CHARV, curr_sym_str()));
		pop_sym();
		factor_value_type = ValueType::CHARV;
	} else if (equal(symbolType::PLUS, symbolType(MINU)) || equal(symbolType::INTCON)) {	// ＜整数＞
		int int_value = Int();
		stack_push(make_shared<ImmediateEntry>(ValueType::INTV, to_string(int_value)));
	} else { error(); }
	output_list_.push_back("<因子>");

	return factor_value_type;
}

/**
＜复合语句＞   ::=  ［＜常量说明＞］［＜变量说明＞］＜语句列＞
*/
void GrammerAnalyzer::CompoundStatement(bool* p_exsit_return, ValueType return_value_type) {

	// 添加四元式:与func_declare使用同一个基本块

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
		IfStatement(p_exsit_return, return_value_type);
		break;
	case symbolType::IDENFR:
		// 函数调用语句| 赋值语句
		if (peek_sym_type(1) == symbolType::ASSIGN || peek_sym_type() == symbolType::LBRACK) {
			AssignStatement();
			checkMissSemi();
		} else if(peek_sym_type() == symbolType::LPARENT) {
			CallFunc();
			checkMissSemi();
		}
		break;
	case symbolType::SCANFTK:
		ReadStatement();
		checkMissSemi();
		break;
	case symbolType::PRINTFTK:
		WriteStatement();
		checkMissSemi();
		break;
	case symbolType::SWITCHTK:
		SwitchStatement(p_exsit_return, return_value_type);
		break;
	case symbolType::SEMICN:
		pop_sym();
		break;
	case symbolType::RETURNTK:
		ReturnStatement(p_exsit_return, return_value_type);
		checkMissSemi();
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
	bool isArray = false;
	vector<shared_ptr<TableEntry>> indexs;
	shared_ptr<TableEntry> p_entry = checkUndefine();
	pop_sym();
	if (equal(symbolType::LBRACK)) { 
		isArray = true;
		pop_sym();
		if (Expr() != ValueType::INTV) { addErrorInfor(ErrorType::IllegalArrayIndexType); }
		indexs.push_back(stack_pop_value());
		checkMissRbrack();

		if (equal(symbolType::LBRACK)) {  // ＜标识符＞'['＜表达式＞']''['＜表达式＞']'
			pop_sym();
			if (Expr() != ValueType::INTV) { addErrorInfor(ErrorType::IllegalArrayIndexType); }
			indexs.push_back(stack_pop_value());
			checkMissRbrack();
		} else { // ＜标识符＞'['＜表达式＞']'

		}
	} else {  // ＜标识符＞

	}
	check(symbolType::ASSIGN);

	// 不能改变常量的值
	if (p_entry && p_entry->entry_type() == EntryType::CONST) {
		addErrorInfor(ErrorType::IllegalAssignmentToConst);
	}

	pop_sym();
	Expr();
	auto right_value = stack_pop_value();
	if (!isArray) {
		im_coder_.addQuater(QuaternionFactory::Assign(p_entry, right_value));
	} else {
		for (auto x : indexs) {
			im_coder_.addQuater(QuaternionFactory::pushArrayIndex(x));
		}
		im_coder_.addQuater(QuaternionFactory::setArrayElem(p_entry, right_value));
	}

	output_list_.push_back("<赋值语句>");
}

/**
＜条件语句＞  ::= if '('＜条件＞')'＜语句＞［else＜语句＞］
*/
void GrammerAnalyzer::IfStatement(bool* p_exsit_return, ValueType return_value_type) {
	auto label_entry1 = make_shared<LabelEntry>(this->new_label());
	
	check(symbolType::IFTK);
	pop_sym();
	check(symbolType::LPARENT);
	pop_sym();
	// 在Condition中生成跳转四元式
	Condition(IFTK, label_entry1);
	checkMissRparent();

	Statement(p_exsit_return, return_value_type);


	if (equal(symbolType::ELSETK)) {
		auto label_entry2 = make_shared<LabelEntry>(this->new_label());
		im_coder_.addQuater(QuaternionFactory::Goto(label_entry2));
		// 生成标签四元式
		im_coder_.addQuater(QuaternionFactory::Label(label_entry1));
		pop_sym();
		Statement(p_exsit_return, return_value_type);
		// 生成标签四元式
		im_coder_.addQuater(QuaternionFactory::Label(label_entry2));
	} else {
		// 生成标签四元式
		im_coder_.addQuater(QuaternionFactory::Label(label_entry1));
	}
	output_list_.push_back("<条件语句>");
}

/**
＜条件＞    ::=  ＜表达式＞＜关系运算符＞＜表达式＞
＜关系运算符＞  ::=  <｜<=｜>｜>=｜!=｜==
表达式需均为整数类型才能进行比较
*/
void GrammerAnalyzer::Condition(symbolType jump_type,shared_ptr<LabelEntry> label_entry) {
	symbolType condition_type = symbolType::EQL;
	ValueType value_type1 = Expr();
	auto  expr1 = stack_pop_value();
	if (equal(LSS, LEQ) || equal(GRE, GEQ) || equal(NEQ, EQL)) {
		condition_type = curr_sym_type();
		pop_sym();
	}else { error(); }
	ValueType value_type2 = Expr();
	auto expr2 = stack_pop_value();

	//添加条件跳转四元式
	auto quater = jump(jump_type, condition_type, label_entry,expr1, expr2);
	im_coder_.addQuater(quater);

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
	
	auto begin_label = make_shared<LabelEntry>(this->new_label());
	auto end_label = make_shared<LabelEntry>(this->new_label());
	
	if (equal(WHILETK)) {
		pop_sym();
		check(LPARENT);
		pop_sym();
		// 生成 设置循环体开始标签的四元式
		im_coder_.addQuater(QuaternionFactory::Label(begin_label));

		Condition(WHILETK, end_label);
		checkMissRparent();
		Statement(p_exsit_return, return_value_type);


	} else if (equal(FORTK)) {
		// for'('＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞')'＜语句＞
		pop_sym();
		check(LPARENT);
		pop_sym();
		// ＜标识符＞＝＜表达式＞
		check(IDENFR);
		shared_ptr<TableEntry> p_entry = checkUndefine();
		stack_push(p_entry);
		pop_sym();
		check(ASSIGN);
		pop_sym();
		Expr();
		im_coder_.addQuater(stack_assign());
		checkMissSemi();
		// 生成 设置循环体开始标签的四元式
		im_coder_.addQuater(QuaternionFactory::Label(begin_label));
		// ＜条件＞
		Condition(FORTK, end_label);
		checkMissSemi();
		// ＜标识符＞＝＜标识符＞(+| -)＜步长＞
		check(IDENFR);
		auto left_idenf = checkUndefine();
		pop_sym();
		check(ASSIGN); pop_sym();
		check(IDENFR); 
		auto right_idenf = checkUndefine();
		pop_sym();
		auto op = curr_sym_type();
		check(PLUS, MINU);
		pop_sym();
		int step = Step();
		auto immediate = make_shared<ImmediateEntry>(ValueType::INTV, to_string(step));
		checkMissRparent();
		// 循环体
		Statement(p_exsit_return, return_value_type);
		// 步长语句的四元式
		if (op == symbolType::PLUS) {
			im_coder_.addQuater(QuaternionFactory::Add(left_idenf, right_idenf, immediate));
		} else if (op == symbolType::MINU) {
			im_coder_.addQuater(QuaternionFactory::Sub(left_idenf, right_idenf, immediate));
		}

	} else { error(); }

	// 无条件跳转到循环体开始的四元式
	im_coder_.addQuater(QuaternionFactory::Goto(begin_label));
	// 设置结束标签的四元式
	im_coder_.addQuater(QuaternionFactory::Label(end_label));
	
	output_list_.push_back("<循环语句>");
}

/**
＜步长＞::= ＜无符号整数＞
*/
int GrammerAnalyzer::Step() {
	int step = UnsignedInt();
	output_list_.push_back("<步长>");
	return step;
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

	ValueType switch_value_type = Expr();
	auto expr = stack_pop_value();
	checkMissRparent();
	check(symbolType::LBRACE);
	pop_sym();

	vector<pair<shared_ptr<LabelEntry>, shared_ptr<ImmediateEntry>>> case_list;
	auto& quater_list = im_coder_.curr_bblock()->get_quater_list();
	// 向顺序容器插入元素可能会使所有指向容器的迭代器，引用和指针失效； 参见PrimerC++ P315
	//auto iter = quater_list.end();
	auto& back_elem = quater_list.back();

	auto endswitch_label = make_shared<LabelEntry>(this->new_label());

	CaseList(p_exsit_return, return_value_type, switch_value_type, case_list, endswitch_label);

	// 插入多个跳转语句的中间代码
	vector<shared_ptr<Quaternion>> jump_list;
	for (auto it : case_list) {
		jump_list.push_back(QuaternionFactory::BEQ(it.first,expr ,it.second));
	}
	auto default_label = make_shared<LabelEntry>(this->new_label());
	jump_list.push_back(QuaternionFactory::Goto(default_label));
	auto it = quater_list.begin();
	for (; (*it).get() != back_elem.get(); it++) {}
	quater_list.insert(it + 1, jump_list.begin(), jump_list.end());

	// 检查是否缺少缺省语句
	if (equal(symbolType::DEFAULTTK)) {
		im_coder_.addQuater(QuaternionFactory::Label(default_label));
		SwitchDefault(p_exsit_return, return_value_type);
	}
	else {
		addErrorInfor(ErrorType::MissDefault);
	}

	// 生成结束标签
	im_coder_.addQuater(QuaternionFactory::Label(endswitch_label));


	check(symbolType::RBRACE);
	pop_sym();
	output_list_.push_back("<情况语句>");
}

/**
＜情况表＞   ::=  ＜情况子语句＞{＜情况子语句＞}
*/
void GrammerAnalyzer::CaseList(bool* p_exsit_return, ValueType return_value_type, ValueType switch_value_type
		, vector<pair<shared_ptr<LabelEntry>, shared_ptr<ImmediateEntry>>>& case_list, shared_ptr<LabelEntry> endswitch_label) {
	CaseStatement(p_exsit_return, return_value_type, switch_value_type, case_list, endswitch_label);
	while (equal(symbolType::CASETK)) {
		CaseStatement(p_exsit_return, return_value_type, switch_value_type, case_list, endswitch_label);
	}
	output_list_.push_back("<情况表>");
}

/**
＜情况子语句＞  ::=  case＜常量＞：＜语句＞
＜常量＞   ::=  ＜整数＞|＜字符＞
*/
void GrammerAnalyzer::CaseStatement(bool* p_exsit_return, ValueType return_value_type, ValueType switch_value_type
		, vector<pair<shared_ptr<LabelEntry>, shared_ptr<ImmediateEntry>>>& case_list, shared_ptr<LabelEntry> endswitch_label) {
	check(symbolType::CASETK);
	pop_sym();
	string value = ConstValue(switch_value_type);

	// 生成case分支标签的中间代码
	auto label_entry = make_shared<LabelEntry>(this->new_label());
	auto inum_entry = make_shared<ImmediateEntry>(switch_value_type, value);
	im_coder_.addQuater(QuaternionFactory::Label(label_entry));
	case_list.push_back(make_pair(label_entry, inum_entry));

	check(symbolType::COLON);
	pop_sym();
	Statement(p_exsit_return, return_value_type);
	output_list_.push_back("<情况子语句>");

	// 生成跳转到switch end的goto中间代码
	im_coder_.addQuater(QuaternionFactory::Goto(endswitch_label));
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
	shared_ptr<TableEntry> p_entry = checkUndefine();
	if (p_entry) {
		if (p_entry->value_type() == ValueType::VOIDV) {
			CallNoReturn(dynamic_pointer_cast<FunctionEntry>(p_entry));
		}
		else {
			shared_ptr<TempEntry> ret;
			CallWithReturn(dynamic_pointer_cast<FunctionEntry>(p_entry), &ret);
		}
	}else {
		// 本行出现未定义的函数调用，不会出现其它错误，直接跳到分号
		while (!equal(symbolType::SEMICN)) {
			pop_sym();
		}
	}
}

/**
＜有返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')'
函数调用时，只能调用在之前已经定义过的函数，对是否有返回值的函数都是如此
*/
void GrammerAnalyzer::CallWithReturn(shared_ptr<FunctionEntry> p_entry, shared_ptr<TempEntry>* ret) {
	check(symbolType::IDENFR);
	string func_name = curr_sym_str();
	pop_sym();
	check(symbolType::LPARENT);
	pop_sym();
	ValueParameterList(p_entry);
	checkMissRparent();

	*ret = make_shared<TempEntry>(string("temp_ret"));;
	im_coder_.addQuater(QuaternionFactory::FuncCall(p_entry));
	im_coder_.addQuater(QuaternionFactory::RetAssign(*ret));

	output_list_.push_back("<有返回值函数调用语句>");
}

/**
＜无返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')'
*/
void GrammerAnalyzer::CallNoReturn(shared_ptr<FunctionEntry> p_entry) {
	check(symbolType::IDENFR);
	string func_name = curr_sym_str();
	pop_sym();
	check(symbolType::LPARENT);
	pop_sym();
	ValueParameterList(p_entry);
	checkMissRparent();

	auto func_call = QuaternionFactory::FuncCall(p_entry);
	im_coder_.addQuater(func_call);

	output_list_.push_back("<无返回值函数调用语句>");
}

/**
＜值参数表＞   ::= ＜表达式＞{,＜表达式＞}｜＜空＞

实参的表达式不能是数组名，可以是数组元素
实参的计算顺序,要求生成的目标码运行结果与Clang8.0.1 编译器运行的结果一致
*/
void GrammerAnalyzer::ValueParameterList(shared_ptr<FunctionEntry> p_entry) {
	vector<ValueType> actual_param_list;
	// 可能')'缺失，所以不能用')'来判断是否为空参数表
	//if (!equal(symbolType::RPARENT) ) {
	if (isExpr()) {
		ValueType value_type = Expr();
		shared_ptr<TableEntry> out = stack_pop_value(); 
		im_coder_.addQuater(QuaternionFactory::FuncParamPush(out));  // 四元式
		actual_param_list.push_back(value_type);

		while (equal(symbolType::COMMA)) {
			pop_sym();
			value_type = Expr();
			out = stack_pop_value();
			im_coder_.addQuater(QuaternionFactory::FuncParamPush(out));  // 四元式
			actual_param_list.push_back(value_type);
		}
	}
	// 一行最多出现一个错误，因此使用if - else if的逻辑
	if (actual_param_list.size() != p_entry->formal_param_num()) {
		addErrorInfor(ErrorType::FuncParamNumUnmatch);
	} else if (!p_entry->checkParamList(actual_param_list)) {
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
	auto p_entry = checkUndefine();

	// 不能给常量赋值
	if (p_entry->entry_type() == EntryType::CONST) {
		addErrorInfor(ErrorType::IllegalAssignmentToConst);
	}

	pop_sym();
	checkMissRparent();

	// 生成四元式
	im_coder_.addQuater(QuaternionFactory::Read(p_entry));

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
	shared_ptr<ImmediateEntry> p_str_entry = nullptr;
	shared_ptr<TableEntry> p_expr_entry = nullptr;

	check(symbolType::PRINTFTK);
	pop_sym();
	check(symbolType::LPARENT);
	pop_sym();
	if (equal(symbolType::STRCON)) {
		p_str_entry = make_shared<ImmediateEntry>(ValueType::STRINGV, curr_sym_str());
		String();
		if (equal(symbolType::COMMA)) {
			pop_sym();
			Expr();
			p_expr_entry = stack_pop_value();
		}
	} else {
		Expr();
		p_expr_entry = stack_pop_value();
	}
	checkMissRparent();

	// 生成四元式
	im_coder_.addQuater(QuaternionFactory::Write(p_str_entry, p_expr_entry));
	
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

	if (return_value_type == ValueType::VOIDV) {
		// 生成四元式
		im_coder_.addQuater(QuaternionFactory::FuncReturn(nullptr));

		if (equal(LPARENT)) {								// 无返回值的函数中若出现了形如return(表达式);或return();的语句
			addErrorInfor(ErrorType::ReturnErrorInVoidFunc);
			pop_sym();
			// 例: return(; 会进入Expr()，之后就可能产生未知错误。
			//if (!equal(symbolType::RPARENT)) {
			if (isExpr()) {
				Expr();
			}
			checkMissRparent();
		}

	}else {
		if (!equal(LPARENT)) {								// 有返回值的函数有形如return;的语句；
			addErrorInfor(ErrorType::ReturnErrorInNonvoidFunc);
		}else {
			pop_sym();
			if (!equal(RPARENT)) {
				ValueType expr_value_type = Expr();
				// 生成四元式
				auto expr = stack_pop_value();
				im_coder_.addQuater(QuaternionFactory::FuncReturn(expr));

				if (expr_value_type != return_value_type) {   // return语句中表达式类型与返回值类型不一致
					addErrorInfor(ErrorType::ReturnErrorInNonvoidFunc);
				}
			}else {												// 或有形如return();的语句；
				addErrorInfor(ErrorType::ReturnErrorInNonvoidFunc);
			}
			checkMissRparent();
		}
	}

	output_list_.push_back("<返回语句>");
}

void inline GrammerAnalyzer::stack_push(shared_ptr<TableEntry> opA) {
	this->stack_.push_back(opA);
}

shared_ptr<TableEntry> inline GrammerAnalyzer::stack_pop_value() {
	auto top = stack_.back();
	stack_.pop_back();
	return top;
}

shared_ptr<Quaternion> GrammerAnalyzer::stack_alu(symbolType alu_type) {
	shared_ptr<Quaternion> quater;
	auto opB = stack_pop_value();
	auto opA = stack_pop_value();

	// 特判 a = +expr这种情况
	if (alu_type == symbolType::PLUS && opA == nullptr) {
		stack_push(opB);
		return nullptr;
	}

	auto result = make_shared<TempEntry>(new_temp());
	switch (alu_type) {
	case symbolType::PLUS:
		if (opA == nullptr) {
			
		} else {
			quater = QuaternionFactory::Add(result, opA, opB);
		}
		break;
	case symbolType::MINU:
		if (opA == nullptr) {
			quater = QuaternionFactory::Neg(result, opB);
		} else {
			quater = QuaternionFactory::Sub(result, opA, opB);
		}
		break;
	case symbolType::MULT:
		quater = QuaternionFactory::Mul(result, opA, opB);
		break;
	case symbolType::DIV:
		quater = QuaternionFactory::Div(result, opA, opB);
		break;
	default:
		break;
	}
	stack_push(result);
	return quater;
}

shared_ptr<Quaternion> GrammerAnalyzer::stack_assign() {
	auto opB = stack_pop_value();
	auto opA = stack_pop_value();
	return QuaternionFactory::Assign(opA, opB);
}

shared_ptr<Quaternion> GrammerAnalyzer::jump(symbolType jump_type, symbolType condition_type,
	shared_ptr<LabelEntry> p_label, shared_ptr<TableEntry> expr1, shared_ptr<TableEntry>expr2) {

	shared_ptr<Quaternion> quater;

	if (jump_type == IFTK || jump_type == WHILETK || jump_type == FORTK) {
		// 条件为假时跳转
		switch (condition_type) {
		case symbolType::EQL:
			quater = QuaternionFactory::BNE(p_label, expr1, expr2);
			break;
		case symbolType::NEQ:
			quater = QuaternionFactory::BEQ(p_label, expr1, expr2);
			break;
		case symbolType::LSS:
			quater = QuaternionFactory::BGE(p_label, expr1, expr2);
			break;
		case symbolType::LEQ:
			quater = QuaternionFactory::BGT(p_label, expr1, expr2);
			break;
		case symbolType::GRE:
			quater = QuaternionFactory::BLE(p_label, expr1, expr2);
			break;
		case symbolType::GEQ:
			quater = QuaternionFactory::BLT(p_label, expr1, expr2);
			break;
		default:
			break;
		}
	} else {
		// do while等，条件为真时跳转
	}
	return quater;

}
