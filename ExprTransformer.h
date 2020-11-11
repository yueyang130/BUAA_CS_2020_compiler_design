#ifndef EXPR_TRANSFORMER_H_
#define EXPR_TRANSFORMER_H_

/*�����ʽת��Ϊ��Ԫʽ����*/

#include<vector>
#include<memory>
#include"BasicBlock.h"
#include<map>
#include<assert.h>

using namespace std;

int compare_op(char op1, char op2);


/*�����ʽת��Ϊ��Ԫʽ����*/
class ExprTransformer {

public:
	ExprTransformer(IMCode& im_coder) : 
		im_coder_(im_coder), cnt_(0) {};
	void push_op(char op);
	/*��������ѹ��ջ��������ʽ��ͷ��һ��+/-,+��,��Ҫ��ǰ�油0(nullptr)*/
	void push_value(shared_ptr<TableEntry> p_entry);
	/*��������ʽ�󣬽�ջ��ʣ���Ԫ�س�ջ*/
	shared_ptr<TableEntry> pop();
	/*���ջ��cnt*/
	void clear();
	/*���ñ��ʽ����*/
	void setValueType(ValueType type) { value_type_ = type;  }

private:
	IMCode& im_coder_;
	int cnt_; // ��ʱ�������

	vector<char> op_stack_; // ������ջ
	vector<shared_ptr<TableEntry>> value_stack_; // ������ջ
	/*�����������������������������ջ*/
	void calulate(char op);
	/*���ʽ����*/
	ValueType value_type_;
};

#endif // !EXPR_TRANSFORMER_H_


