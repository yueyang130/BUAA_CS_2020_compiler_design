#ifndef EXPR_TRANSFORMER_H_
#define EXPR_TRANSFORMER_H_

/*将表达式转化为四元式的类*/

#include<vector>
#include<memory>
#include"BasicBlock.h"
#include<map>
#include<assert.h>

using namespace std;

int compare_op(char op1, char op2);


/*将表达式转化为四元式的类*/
class ExprTransformer {

public:
	ExprTransformer(IMCode& im_coder) : 
		im_coder_(im_coder), cnt_(0) {};
	void push_op(char op);
	/*将操作数压入栈，如果表达式开头是一个+/-,+号,则要在前面补0(nullptr)*/
	void push_value(shared_ptr<TableEntry> p_entry);
	/*输入完表达式后，将栈中剩余的元素出栈*/
	shared_ptr<TableEntry> pop();
	/*清空栈和cnt*/
	void clear();
	/*设置表达式类型*/
	void setValueType(ValueType type) { value_type_ = type;  }

private:
	IMCode& im_coder_;
	int cnt_; // 临时变量编号

	vector<char> op_stack_; // 操作符栈
	vector<shared_ptr<TableEntry>> value_stack_; // 操作数栈
	/*弹出两个操作数，计算后放入操作数栈*/
	void calulate(char op);
	/*表达式类型*/
	ValueType value_type_;
};

#endif // !EXPR_TRANSFORMER_H_


