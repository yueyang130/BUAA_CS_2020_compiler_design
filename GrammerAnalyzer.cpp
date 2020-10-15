#include "GrammerAnalyzer.h"

GrammerAnalyzer::GrammerAnalyzer(LexicalAnalyzer& my_lexical_analyzer)
	:sym_infor_list_(my_lexical_analyzer.sym_infor_list_), lexical_analyzer_(my_lexical_analyzer) {

	tk_idx_ = 0;
	output_list_.reserve(INIT_LENGTH);
}

GrammerAnalyzer& GrammerAnalyzer::getInstance(LexicalAnalyzer& my_lexical_analyzer) {
	static GrammerAnalyzer instance(my_lexical_analyzer);
	return instance;
}

void GrammerAnalyzer::analyzeGrammer() {
	Program();
}

/** 
*									�﷨��������
* 0. ����ж��ѡ�������ϲ��first���м�飻���ֻ��һ��ѡ�����ڱ����ٶ�first���м�顣
* 1.��֤����ÿ���ֳ���ʱ��sym_idx_ָ��ֳ����Ӧ�ĵ�һ��sym
* 2.���˳��ֳ���ǰ����Ҫ����Ĵʷ��ɷ���ӵ�output_list_
*  ����sym_idxָ��ֳ����Ӧ�﷨�ɷֺ�ĵ�һ��sym
*/

/**
���ַ�����   ::=  "��ʮ���Ʊ���Ϊ32,33,35-126��ASCII�ַ���" 

�ַ�����Ҫ��������һ���ַ�
*/
void GrammerAnalyzer::String() {
	check(symbolType::STRCON);
	pop_sym();
	output_list_.push_back("�ַ���");
}

/**
* 1. ������    ::= �ۣ�����˵�����ݣۣ�����˵������{���з���ֵ�������壾|���޷���ֵ�������壾}���������� 
* 2. ͨ��Ԥ���ж��� <�з���ֵ�ĺ�������> ����  <��������>
* <��������> = ��int|char) <��ʶ��> ('['|,|;|=)
* <�з���ֵ�ĺ�������> = (int|char) <��ʶ��> '('
*/
void GrammerAnalyzer::Program() {
	if (sym_infor_list_.size() == 0) {
		return;
	}
	
	if (equal(symbolType::CONSTTK)) {
		ConstDeclare();
	}

	if (equal(CONSTTK, CHARTK)) {
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
	output_list_.push_back("����");
}

/**
* ������˵���� ::=  const���������壾;{ const���������壾;}
*/
void GrammerAnalyzer::ConstDeclare() {
	pop_sym();
	ConstDefine();
	while (equal(symbolType::CONSTTK)) {
		pop_sym();
		ConstDefine();
	}
	check(symbolType::SEMICN);
	pop_sym();
	output_list_.push_back("����˵��");
}


/**
* ���������壾   ::=   int����ʶ��������������{,����ʶ��������������}  
                  | char����ʶ���������ַ���{,����ʶ���������ַ���}  
*/
void GrammerAnalyzer::ConstDefine() {
	if (equal(symbolType::INTTK)) {
		pop_sym();
		if (equal(symbolType::IDENFR)) {
			pop_sym();
			if (equal(symbolType::ASSIGN)) {
				pop_sym();
				Int();
			}
		}
		while (equal(symbolType::COMMA)) {
			if (!equal(symbolType::IDENFR)) { error(); }
			else {
				pop_sym();
				if (!equal(symbolType::ASSIGN)) { error(); }
				else {
					pop_sym();
					Int();
				}
			}
		}
	} else if (equal(symbolType::CHARTK)) {  
		pop_sym();
		if (equal(symbolType::IDENFR)) {
			pop_sym();
			if (equal(symbolType::ASSIGN)) {
				pop_sym();
				Char();
			}
		}
		while (equal(symbolType::COMMA)) {
			check(symbolType::IDENFR);
			pop_sym();
			check(symbolType::ASSIGN);
			pop_sym();
			Char();
		}
	}

	output_list_.push_back("��������");

}

/**
���޷���������  ::= �����֣��������֣���
*/
void GrammerAnalyzer::UnsignedInt() {
	check(INTCON);
	pop_sym();
	output_list_.push_back("�޷�������");
}

/**
* ��������        ::= �ۣ������ݣ��޷���������
*/
void GrammerAnalyzer::Int() {
	if (equal(symbolType::PLUS, symbolType::MINU)) {
		pop_sym();
	}
	UnsignedInt();
	output_list_.push_back("����");
}


void GrammerAnalyzer::Identifier() {
	check(symbolType::IDENFR);
	pop_sym();
}

/**
������˵����  ::= ���������壾;{���������壾;}
*/
void GrammerAnalyzer::VarDeclare() {
	VarDefine();
	check(symbolType::SEMICN);
	pop_sym();
	// �ж���<��������> ���� <�з���ֵ��������>
	if (equal(symbolType::INTTK, symbolType::CHARTK)) {
		if (peek_sym_type(1) == symbolType::IDENFR) {
			symbolType peek2 = peek_sym_type(2);
			if (peek2 == symbolType::LBRACK || peek2 == symbolType::SEMICN ||
				peek2 == symbolType::COMMA || peek2 == symbolType::ASSIGN) {
				VarDefine();
				check(SEMICN);
			}
			
		}
	}
	output_list_.push_back("����˵��");
}

/**
* ���������壾 ::= �����������޳�ʼ����|���������弰��ʼ����
*/
void GrammerAnalyzer::VarDefine() {
	check(INTTK, CHARTK);

	// ͨ���ڷֺ�ǰ�Ƿ���ֵȺ��ж��Ƿ��ʼ��
	bool no_init = true;
	for (int i = 2; peek_sym_type(i) != symbolType::SEMICN; i++) {
		if (peek_sym_type(i) == symbolType::ASSIGN) {
			no_init = false;
		}
	}
	if (no_init) { VarDefineNoInit(); }
	else { VarDefineWithInit(); }
	

	output_list_.push_back("��������");
}

/**
* �����������޳�ʼ����  ::= �����ͱ�ʶ����(����ʶ����
	|����ʶ����'['���޷���������']'
	|����ʶ����'['���޷���������']''['���޷���������']')
	{,(����ʶ����|����ʶ����'['���޷���������']'|����ʶ����'['���޷���������']''['���޷���������']' )}
*/
void GrammerAnalyzer::VarDefineNoInit() {
	check(INTTK, CHARTK);
	pop_sym();
	VarDefineType();
	while (equal(COMMA)) {
		pop_sym();
		VarDefineType();
	}

	output_list_.push_back("���������޳�ʼ��");
}

/**
* (����ʶ����|
  ����ʶ����'['���޷���������']'|
  ����ʶ����'['���޷���������']''['���޷���������']'
*/
void GrammerAnalyzer::VarDefineType() {
	check(IDENFR);
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
* ���������弰��ʼ����  ::= �����ͱ�ʶ��������ʶ����=��������|
	�����ͱ�ʶ��������ʶ����'['���޷���������']'='{'��������{,��������}'}'|
	�����ͱ�ʶ��������ʶ����'['���޷���������']''['���޷���������']'='{''{'��������{,��������}'}'{, '{'��������{,��������}'}'}'}'

* 	�г�ʼ���ı�������һ��ֻ�ܶ�һ����ʶ�����г�ʼ��
*/
void GrammerAnalyzer::VarDefineWithInit() {
	check(INTTK, CHARTK);
	symbolType var_type = curr_sym_type();
	pop_sym();
	check(IDENFR);
	pop_sym();
	if (equal(symbolType::ASSIGN)) {
		pop_sym();
		if (var_type == symbolType::INTTK) { UnsignedInt(); }
		else if (var_type == symbolType::CHARTK) { check(symbolType::CHARCON); pop_sym(); }
		else { error(); }

	} else if (equal(symbolType::LBRACK)) {
		pop_sym();
		UnsignedInt();
		check(RBRACK);
		pop_sym();
		if (equal(symbolType::LBRACK)) {
			pop_sym();
			UnsignedInt();
			check(RBRACK);
			pop_sym();
		}

	} else { error(); }

	output_list_.push_back("�������弰��ʼ��");
}

/**
���з���ֵ�������壾  ::=  ������ͷ����'('��������')' '{'��������䣾'}'
*/
void GrammerAnalyzer::FuncDefineWithReturn() {
	FuncDefineHead();
	check(symbolType::LPARENT);
	pop_sym();
	ParameterList();
	pop_sym();
	check(symbolType::RPARENT);
	pop_sym();
	check(symbolType::LBRACE);
	pop_sym();
	CompoundStatement();
	check(symbolType::RBRACE);
	pop_sym();
	output_list_.push_back("�з���ֵ��������");
}

/**
���޷���ֵ�������壾  ::= void����ʶ����'('��������')''{'��������䣾'}'
*/
void GrammerAnalyzer::FuncDefineNoReturn() {
	check(symbolType::VOIDTK);
	pop_sym();
	check(symbolType::IDENFR);
	func_no_ret_list_.push_back(curr_sym_str());
	pop_sym();
	check(symbolType::LPARENT);
	pop_sym();
	ParameterList();
	check(symbolType::RPARENT);
	pop_sym();
	check(symbolType::LBRACE);
	pop_sym();
	CompoundStatement();
	check(symbolType::RBRACE);
	pop_sym();
	output_list_.push_back("�޷���ֵ��������");
}

/**
*������ͷ����   ::=  int����ʶ���� |char����ʶ���� 
*/
void GrammerAnalyzer::FuncDefineHead() {
	check(symbolType::INTTK, symbolType::CHARTK);
	pop_sym();
	check(IDENFR);
	func_with_ret_list_.push_back(curr_sym_str());
	pop_sym();
	output_list_.push_back("����ͷ��");
}

/**
��������    ::=  �����ͱ�ʶ��������ʶ����{,�����ͱ�ʶ��������ʶ����}| ���գ�
�����ͱ�ʶ����      ::=  int | char  
*/
void GrammerAnalyzer::ParameterList() {
	if (equal(INTTK, CHARTK)) {
		pop_sym();
		check(symbolType::IDENFR);
		pop_sym();
		while (equal(symbolType::COMMA)) {
			pop_sym();
			check(INTTK, CHARTK);
			pop_sym();
			check(symbolType::IDENFR);
			pop_sym();
		}
	}
	output_list_.push_back("������");
}

/**
����������    ::= void main��(����)�� ��{����������䣾��}��
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
	CompoundStatement();
	check(symbolType::RBRACE);
	pop_sym();
	output_list_.push_back("������");
}

/**
�����ʽ��    ::= �ۣ������ݣ��{���ӷ�����������}
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
	output_list_.push_back("���ʽ");
}

/**
���     ::= �����ӣ�{���˷�������������ӣ�}
*/
void GrammerAnalyzer::Item() {
	Factor();
	while (equal(symbolType::MULT, symbolType::DIV)) {
		pop_sym();
		Factor();
	}
	output_list_.push_back("��");
}


/**
�����ӣ�    ::= ����ʶ����
				������ʶ����'['�����ʽ��']'
				 |����ʶ����'['�����ʽ��']''['�����ʽ��']'
				 |'('�����ʽ��')'
			     ����������
				 |���ַ���
				�����з���ֵ����������䣾

��������        ::= �ۣ������ݣ��޷���������
���з���ֵ����������䣾 ::= ����ʶ����'('��ֵ������')'

1.����ʶ����'['�����ʽ��']'�ͣ���ʶ����'['�����ʽ��']''['�����ʽ��']'�еģ����ʽ��ֻ�������ͣ��±��0��ʼ

2.��������ʶ�����������������������鲻������μ����㣬����Ԫ�ؿ��Բμ�����
*/
void GrammerAnalyzer::Factor() {
	if (equal(symbolType::IDENFR)) {
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

		} else if (equal(symbolType::LPARENT)) {   // �з���ֵ��������
			pop_sym();
			ValueParameterList();
			check(symbolType::RPARENT);
			pop_sym();
		}

	} else if (equal(symbolType::LPARENT)) {
		pop_sym();
		Expr();
		check(RPARENT);
		pop_sym();

	} else if (equal(symbolType::CHARCON)) {
		pop_sym();
	} else if (equal(symbolType::PLUS, symbolType(MINU)) || equal(symbolType::INTCON)) {
		Int();
	} else { error(); }
	output_list_.push_back("����");
}

/**
��������䣾   ::=  �ۣ�����˵�����ݣۣ�����˵�����ݣ�����У�
*/
void GrammerAnalyzer::CompoundStatement() {
	if (equal(symbolType::CONSTTK)) {
		ConstDeclare();
	}
	if (equal(symbolType::INTTK, symbolType::CHARTK)) {
		VarDeclare();
	}
	StatetmentList();
	output_list_.push_back("�������");

}

/**
������У�   ::= ������䣾��
�����{}ָ����0�������������'{'
*/
void GrammerAnalyzer::StatetmentList() {
	while (!equal(symbolType::RBRACE)) {
		Statement();
	}
	output_list_.push_back("�����");
}

/**
����䣾    ::= ��ѭ����䣾����������䣾|
			���з���ֵ����������䣾;  |���޷���ֵ����������䣾;��
			����ֵ��䣾;��������䣾;����д��䣾;���������䣾��
			���գ�;|��������䣾; | '{'������У�'}'
*/
void GrammerAnalyzer::Statement() {
	switch (curr_sym_type()) {
	case symbolType::WHILETK:
		LoopStatement();
		break;
	case symbolType::FORTK:
		LoopStatement();
		break;
	case symbolType::IFTK:
		ConditionalStatement();
		break;
	case symbolType::IDENFR:
		if (peek_sym_type(1) == symbolType::ASSIGN) {
			AssignStatement();
			check(symbolType::SEMICN);
			pop_sym();
		} else {
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
		SwitchStatement();
		break;
	case symbolType::SEMICN:
		pop_sym();
		break;
	case symbolType::RETURNTK:
		ReturnStatement();
		check(symbolType::SEMICN);
		pop_sym();
		break;
	case symbolType::LBRACE:
		pop_sym();
		StatetmentList();
		check(symbolType::RBRACE);
	default:
		error();
		break;
	}
	output_list_.push_back("���");
}

/**
����ֵ��䣾   ::=  ����ʶ�����������ʽ��
		|����ʶ����'['�����ʽ��']'=�����ʽ��
		|����ʶ����'['�����ʽ��']''['�����ʽ��']' =�����ʽ��

����ʶ�����������ʽ���е�<��ʶ��>����Ϊ��������������
*/
void GrammerAnalyzer::AssignStatement() {
	check(symbolType::IDENFR);
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
	output_list_.push_back("��ֵ���");
}

/**
��������䣾  ::= if '('��������')'����䣾��else����䣾��
*/
void GrammerAnalyzer::ConditionalStatement() {
	check(symbolType::IFTK);
	pop_sym();
	check(symbolType::LPARENT);
	pop_sym();
	Condition();
	check(symbolType::RPARENT);
	pop_sym();
	Statement();
	if (equal(symbolType::ELSETK)) {
		pop_sym();
		Statement();
	}
	output_list_.push_back("�������");
}

/**
��������    ::=  �����ʽ������ϵ������������ʽ��
����ϵ�������  ::=  <��<=��>��>=��!=��==
���ʽ���Ϊ�������Ͳ��ܽ��бȽ�
*/
void GrammerAnalyzer::Condition() {
	// TODO: ���ʽ���Ϊ�������Ͳ��ܽ��бȽ�
	Expr();
	if (equal(LSS, LEQ) || equal(GRE, GEQ) || equal(NEQ, EQL)) {
		pop_sym();
	}else { error(); }
	Expr();
	output_list_.push_back("����");
}

/**
��ѭ����䣾   ::=  while '('��������')'����䣾
	| for'('����ʶ�����������ʽ��;��������;����ʶ����������ʶ����(+|-)��������')'����䣾
*/
void GrammerAnalyzer::LoopStatement() {
	if (equal(WHILETK)) {
		pop_sym();
		check(LPARENT);
		pop_sym();
		Condition();
		check(RPARENT);
		pop_sym();
		Statement();

	} else if (equal(FORTK)) {
		pop_sym();
		check(LPARENT);
		pop_sym();
		check(IDENFR);
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
		pop_sym();
		check(PLUS, MINU);
		pop_sym();
		Step();
		check(RPARENT);
		pop_sym();
		Statement();

	} else { error(); }
	
	output_list_.push_back("ѭ�����");
}

/**
��������::= ���޷���������
*/
void GrammerAnalyzer::Step() {
	UnsignedInt();
	output_list_.push_back("����");
}

/**
�������䣾  ::=  switch ��(�������ʽ����)�� ��{�����������ȱʡ����}��

�������У�switch����ı��ʽ��case����ĳ���ֻ�������int��char���ͣ�
ÿ����������ִ����Ϻ󣬲�����ִ�к������������
*/
void GrammerAnalyzer::SwitchStatement() {
	check(symbolType::SWITCHTK);
	pop_sym();
	check(symbolType::LPARENT);
	pop_sym();
	Expr();
	check(symbolType::RPARENT);
	pop_sym();
	check(symbolType::RBRACE);
	pop_sym();
	CaseList();
	SwitchDefault();
	check(symbolType::RBRACE);
	pop_sym();
	output_list_.push_back("������");
}

/**
�������   ::=  ���������䣾{���������䣾}
*/
void GrammerAnalyzer::CaseList() {
	CaseStatement();
	while (equal(symbolType::CASETK)) {
		CaseStatement();
	}
	output_list_.push_back("�����");
}

/**
���������䣾  ::=  case��������������䣾
��������   ::=  ��������|���ַ���
*/
void GrammerAnalyzer::CaseStatement() {
	// TODO: ��鳣����switch�����Ƿ�ƥ��
	check(symbolType::CASETK);
	pop_sym();
	if (equal(symbolType::CHARCON)) {
		pop_sym();
	} else {
		Int();
	}
	check(symbolType::COLON);
	pop_sym();
	Statement();
	output_list_.push_back("��������");
}

/**
��ȱʡ��   ::=  default :����䣾
*/
void GrammerAnalyzer::SwitchDefault() {
	check(symbolType::DEFAULTTK);
	pop_sym();
	Statement();
	output_list_.push_back("ȱʡ");
}

void GrammerAnalyzer::CallFunc() {
	check(symbolType::IDENFR);
	string& func_name = curr_sym_str();
	for (auto& x : func_with_ret_list_) {
		if (x == func_name) {
			CallWithReturn();
			return;
		}
	}
	for (auto& x : func_no_ret_list_) {
		if (x == func_name) {
			CallNoReturn();
			return;
		}
	}
	error();
}

/**
���з���ֵ����������䣾 ::= ����ʶ����'('��ֵ������')'
��������ʱ��ֻ�ܵ�����֮ǰ�Ѿ�������ĺ��������Ƿ��з���ֵ�ĺ����������
*/
void GrammerAnalyzer::CallWithReturn() {
	check(symbolType::IDENFR);
	pop_sym();
	check(symbolType::LPARENT);
	pop_sym();
	ValueParameterList();
	check(RPARENT);
	pop_sym();
	output_list_.push_back("�з���ֵ�����������");
}

/**
���޷���ֵ����������䣾 ::= ����ʶ����'('��ֵ������')'
*/
void GrammerAnalyzer::CallNoReturn() {
	check(symbolType::IDENFR);
	pop_sym();
	check(symbolType::LPARENT);
	pop_sym();
	ValueParameterList();
	check(RPARENT);
	pop_sym();
	output_list_.push_back("�޷���ֵ�����������");
}

/**
��ֵ������   ::= �����ʽ��{,�����ʽ��}�����գ�

ʵ�εı��ʽ������������������������Ԫ��
ʵ�εļ���˳��,Ҫ�����ɵ�Ŀ�������н����Clang8.0.1 ���������еĽ��һ��
*/
void GrammerAnalyzer::ValueParameterList() {
	if (!equal(symbolType::RPARENT)) {
		Expr();
		while (equal(symbolType::COMMA)) {
			pop_sym();
			Expr();
		}
	}
	output_list_.push_back("ֵ������");
}

/**
������䣾    ::=  scanf '('����ʶ����')'

�ӱ�׼�����ȡ<��ʶ��>��ֵ���ñ�ʶ�������ǳ�������������
���ɵ�PCODE��MIPS���������ʱ����ÿһ��scanf��䣬���۱�ʶ����������char����int��ĩβ����س�����testin.txt�ļ��е���������Ҳ��ÿ����һ��
����MIPS���ʱ����syscallָ����÷�ʹ��
*/
void GrammerAnalyzer::ReadStatement() {
	check(symbolType::SCANFTK);
	pop_sym();
	check(symbolType::LPARENT);
	pop_sym();
	check(symbolType::IDENFR);
	pop_sym();
	check(symbolType::RPARENT);
	pop_sym();

	output_list_.push_back("�����");
}

/**
��д��䣾    ::= printf '(' ���ַ�����,�����ʽ�� ')'| printf '('���ַ����� ')'| printf '('�����ʽ��')'

printf '(' ���ַ�����,�����ʽ�� ')'���ʱ��������ַ��������ݣ���������ʽ��ֵ������֮���޿ո�
���ʽΪ�ַ���ʱ������ַ���Ϊ����ʱ�������
���ַ�����ԭ�������������ת�壩
ÿ��printf�������������һ�У�����β�л��з�\n����
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
	
	output_list_.push_back("д���");
}

/**
��������䣾   ::=  return['('�����ʽ��')']

�޷���ֵ�ĺ����п���û��return��䣬Ҳ����������return;�����
�з���ֵ�ĺ���ֻҪ����һ��������ֵ��return��䣨���ʽ��С���ţ����ɣ����ü��ÿ����֧�Ƿ��д�����ֵ��return���
*/
void GrammerAnalyzer::ReturnStatement() {
	// ���return����뺯������ֵ���͵�ƥ��
	check(symbolType::RETURNTK);
	pop_sym();
	if (equal(symbolType::LPARENT)) {
		pop_sym();
		Expr();
		check(symbolType::RPARENT);
		pop_sym();
	}

	output_list_.push_back("���������");
}
