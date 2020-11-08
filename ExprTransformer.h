#ifndef EXPR_TRANSFORMER_H_
#define EXPR_TRANSFORMER_H_

#include<vector>
#include<memory>
#include"SymbolTable.h"
#include"Block.h"
#include<map>
#include<assert.h>

using namespace std;

int compare_op(char op1, char op2) {
	static map<char, int> op2level = {
		{'+', 10},
		{'-', 10},
		{'*', 20},
		{'/', 20}
	};
	int level1 = op2level.at(op1);
	int level2 = op2level.at(op2);
	if (level1 < level2) {
		return -1;
	} else if (level1 == level2) {
		return 0;
	} else {
		return 1;
	}
}


/*将表达式转化为四元式的类*/
class ExprTransformer {

public:
	ExprTransformer(IMCode& im_coder, SymbolTable& sym_table) : 
		im_coder_(im_coder), sym_table_(sym_table) {};
	void push_op(char op);
	void push_value(shared_ptr<TableEntry> p_entry);
	/*输入完表达式后，将栈中剩余的元素出栈*/
	shared_ptr<TableEntry> pop();

private:
	IMCode& im_coder_;
	SymbolTable& sym_table_;

	vector<char> op_stack_; // 操作符栈
	vector<shared_ptr<TableEntry>> value_stack_; // 操作数栈
	/*弹出两个操作数，计算后放入操作数栈*/
	void calulate(char op);
};




#endif // !EXPR_TRANSFORMER_H_


