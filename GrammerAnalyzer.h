#ifndef GRAMMER_ANALYZER_H_
#define GRAMMER_ANALYZER_H_

#include "LexicalAnalyzer.h"
#include <iostream>
using namespace std;



class GrammerAnalyzer {

public:
	static GrammerAnalyzer& getInstance(LexicalAnalyzer&);
	void analyzeGrammer();

	/*�������*/
	void show(ostream& fout);


private:
	GrammerAnalyzer(LexicalAnalyzer&);

	int tk_idx_;
	vector<SymInfor>& sym_infor_list_;
	LexicalAnalyzer& lexical_analyzer_;

	/*�������б�*/
	vector<string> output_list_;

	/*�м�����¼*/
	vector<string> func_with_ret_list_;
	vector<string> func_no_ret_list_;
	
	/*֧�ֺ���*/
	inline void pop_sym() {
		output_list_.push_back(lexical_analyzer_.type_to_str(sym_infor_list_[tk_idx_].type) + " " + sym_infor_list_[tk_idx_].sym);
		tk_idx_++;
	}
	inline symbolType curr_sym_type() { return sym_infor_list_[tk_idx_].type; }
	inline string& curr_sym_str() { return sym_infor_list_[tk_idx_].sym; }
	inline symbolType peek_sym_type(int offset = 1) { return sym_infor_list_[tk_idx_ + offset].type; }
	inline bool isFunctionWithReturn() { 
		return (curr_sym_type() == symbolType::INTTK || curr_sym_type() == symbolType::CHARTK)
			&& peek_sym_type(1) == symbolType::IDENFR && peek_sym_type(2) == symbolType::LPARENT;
	}
	inline bool isFunctionNoReturn() {
		return curr_sym_type() == symbolType::VOIDTK && peek_sym_type(1) == symbolType::IDENFR;
	}
	inline bool equal(symbolType ref) { return curr_sym_type() == ref; }
	inline bool equal(symbolType ref1, symbolType ref2) { return curr_sym_type() == ref1 || curr_sym_type() == ref2; }
	
	/*��������*/
	void error() { cout << "error!"; }
	inline void check(symbolType ref) {
		if (!equal(ref)) { error(); }
	}
	inline void check(symbolType ref1, symbolType ref2) {
		if (!equal(ref1, ref2)) { error(); }
	}

	/*���ս���ķ�������*/
	void String();
	void Program();

	void RepeatIfMark(void (GrammerAnalyzer::*handler)(symbolType), symbolType var_type, symbolType mark = COMMA);
	//void RepeatIfMark(void (GrammerAnalyzer::*handler)(void), symbolType mark = COMMA);
	//void RepeatIfMark(symbolType, symbolType mark = COMMA);


	void ConstDeclare();
	void ConstDefine();
	void UnsignedInt();
	void Int();
	//void Char();
	void Identifier();

	void ConstValue(symbolType vartype);   // ����<�������弰��ʼ��>��<��������>
	void ConstValue();   // ��ʱ���ý�������ƥ����ʱʹ�õĺ���

	void VarDeclare();
	void VarDefine();
	void VarDefineNoInit();
	void VarDefineType();
	void VarDefineWithInit();

	void FuncDefineWithReturn();
	void FuncDefineNoReturn();
	void FuncDefineHead();
	void ParameterList();

	void Main();
	void Expr();
	void Item();
	void Factor();

	void CompoundStatement();
	void StatetmentList();
	void Statement();
	void AssignStatement();
	void ConditionalStatement();
	void Condition();
	void LoopStatement();
	void Step();
	void SwitchStatement();
	void CaseList();
	void CaseStatement();
	void SwitchDefault();
	void CallFunc();
	void CallWithReturn();
	void CallNoReturn();
	void ValueParameterList();
	void ReadStatement();
	void WriteStatement();
	void ReturnStatement();

};

#endif // !GRAMMER_ANALYZER_H_




