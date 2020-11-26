#ifndef GRAMMER_ANALYZER_H_
#define GRAMMER_ANALYZER_H_

#include "LexicalAnalyzer.h"
#include "SymbolTable.h"
#include "BasicBlock.h"
#include <iostream>
using namespace std;



class GrammerAnalyzer {

public:
	static GrammerAnalyzer& getInstance(LexicalAnalyzer&, IMCode&);
	void analyzeGrammer();

	/*�������*/
	void show(ostream& fout);
	void showError(ostream& fout);


private:
	GrammerAnalyzer(LexicalAnalyzer&, IMCode&);

	/*ָ��ǰҪ�����﷨������sym*/
	int tk_idx_;
	/*�洢�ʷ�����������б�*/
	vector<SymInfor>& sym_infor_list_;
	/*������Ϣ�б�*/
	vector<ErrorInfor>& error_infor_list_;
	/*�ʷ�������*/
	LexicalAnalyzer& lexical_analyzer_;
	/*�м����������*/
	IMCode& im_coder_;
	/*ջʽ�����*/
	vector<shared_ptr<TableEntry>> stack_;
	/*�������б�*/
	vector<string> output_list_;

	/*���ű�*/
	SymbolTable& sym_table_;
	
	/*֧�ֺ���*/
	inline void pop_sym() {
		output_list_.push_back(lexical_analyzer_.type_to_str(sym_infor_list_[tk_idx_].type) + " " + sym_infor_list_[tk_idx_].sym);
		tk_idx_++;
	}
	inline symbolType curr_sym_type() { return sym_infor_list_[tk_idx_].type; }
	inline string& curr_sym_str() { return sym_infor_list_[tk_idx_].sym; }
	inline int curr_sym_row() { return sym_infor_list_[tk_idx_].row; }
	inline symbolType peek_sym_type(int offset = 1) { return sym_infor_list_[tk_idx_ + offset].type; }
	inline int peek_sym_row(int offset = 1) { return sym_infor_list_[tk_idx_ + offset].row; }

	inline bool isFunctionWithReturn() { 
		return (curr_sym_type() == symbolType::INTTK || curr_sym_type() == symbolType::CHARTK)
			&& peek_sym_type(1) == symbolType::IDENFR && peek_sym_type(2) == symbolType::LPARENT;
	}
	inline bool isFunctionNoReturn() {
		return curr_sym_type() == symbolType::VOIDTK && peek_sym_type(1) == symbolType::IDENFR;
	}
	inline bool isExpr() { return equal(PLUS, MINU) || equal(IDENFR) || equal(LPARENT) || equal(INTCON) || equal(CHARCON);   }

	inline bool equal(symbolType ref) { return curr_sym_type() == ref; }
	inline bool equal(symbolType ref1, symbolType ref2) { return curr_sym_type() == ref1 || curr_sym_type() == ref2; }

	inline void addErrorInfor(ErrorType error_type, int adjust_row) {
		ErrorInfor error_infor = {  adjust_row, error_type, curr_sym_str() };
		error_infor_list_.push_back(error_infor);
	}

	inline void addErrorInfor(ErrorType error_type) {
		ErrorInfor error_infor = { curr_sym_row(), error_type, curr_sym_str() };
		error_infor_list_.push_back(error_infor);
	}
	
	/*��������*/
	void error() { cout << "error!"; }
	inline void check(symbolType ref) {
		if (!equal(ref)) { error(); }
	}
	inline void check(symbolType ref1, symbolType ref2) {
		if (!equal(ref1, ref2)) { error(); }
	}
	shared_ptr<TableEntry> checkUndefine();
	void checkMissSemi(); /*����Ƿֺţ�pop_sym; ������������Ϣ�б�*/
	void checkMissRparent();
	void checkMissRbrack(); 

	/*���ս���ķ�������*/
	void String();
	void Program();

	int RepeatIfMark(string (GrammerAnalyzer::*handler)(ValueType), ValueType var_type, symbolType mark = COMMA, vector<string>* ptr_elems = nullptr);
	//void RepeatIfMark(void (GrammerAnalyzer::*handler)(void), symbolType mark = COMMA);
	//void RepeatIfMark(symbolType, symbolType mark = COMMA);


	void ConstDeclare();
	void ConstDefine();
	void ConstDefineForInt();
	void ConstDefineForChar();
	int UnsignedInt();
	int Int();
	void Identifier();

	string ConstValue(ValueType vartype);   // ����<�������弰��ʼ��>��<��������>
	string ConstValue();   // ��ʱ���ý�������ƥ����ʱʹ�õĺ���

	void VarDeclare();
	void VarDefine();
	void VarDefineNoInit();
	void VarDefineType(ValueType entry_value_type);
	void VarDefineWithInit();

	void FuncDefineWithReturn();
	void FuncDefineNoReturn();
	void FuncDefineHead(string& func_name, ValueType& value_type);
	void ParameterList(vector<ValueType>& formal_param_list);

	void Main();
	ValueType Expr();
	ValueType Item();
	ValueType Factor();

	void CompoundStatement(bool* p_exist_return, ValueType return_value_type);
	void StatetmentList(bool* p_exist_return, ValueType return_value_type);
	void Statement(bool* p_exist_return, ValueType return_value_type);
	void AssignStatement();
	void IfStatement(bool* p_exsit_return, ValueType return_value_type);
	void Condition(symbolType jump_type, shared_ptr<LabelEntry> label_entry);
	void LoopStatement(bool* p_exsit_return, ValueType return_value_type);
	int Step();
	void SwitchStatement(bool* p_exsit_return, ValueType return_value_type);
	void CaseList(bool* p_exsit_return, ValueType return_value_type, ValueType switch_value_type
			, vector<pair<shared_ptr<LabelEntry>, shared_ptr<ImmediateEntry>>>& case_list, shared_ptr<LabelEntry> endswitch_label);
	void CaseStatement(bool* p_exsit_return, ValueType return_value_type, ValueType switch_value_type
			, vector<pair<shared_ptr<LabelEntry>, shared_ptr<ImmediateEntry>>>& case_list, shared_ptr<LabelEntry> endswitch_label);
	void SwitchDefault(bool* p_exsit_return, ValueType return_value_type);
	void CallFunc();
	void CallWithReturn(shared_ptr<FunctionEntry> p_entry, shared_ptr<TempEntry>* ret);
	void CallNoReturn(shared_ptr<FunctionEntry> p_entry);
	void ValueParameterList(shared_ptr<FunctionEntry> p_entry);
	void ReadStatement();
	void WriteStatement();
	void ReturnStatement(bool* p_exsit_return, ValueType return_value_type);

	/*�м��������֧�ֺ���*/
	// ��ջ�Ĳ���
	shared_ptr<TableEntry> stack_pop_value();
	void stack_push(shared_ptr<TableEntry> opA);
	shared_ptr<Quaternion> stack_alu(symbolType alu_type);
	shared_ptr<Quaternion> stack_assign();

	int temp_cnt = 0;
	string new_temp() { return "@t" + to_string(temp_cnt++); }

	/*������ת����(����Ϊ����ת/����Ϊ����ת)����������(==,!=,>,...)������ת����Ԫʽ*/
	shared_ptr<Quaternion> jump(symbolType jump_type, symbolType condition_type, 
		shared_ptr<LabelEntry> p_label, shared_ptr<TableEntry> expr1, shared_ptr<TableEntry>expr2);
	/*���һ��û���ù��ı�ǩ��*/
	int label_cnt = 0;
	string new_label() { return "label" + to_string(++label_cnt); }
};



#endif // !GRAMMER_ANALYZER_H_




