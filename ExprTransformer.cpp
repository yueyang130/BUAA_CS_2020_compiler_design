#include "ExprTransformer.h"

//TODO: ��ʱ������value_type,��Ӱ��print�Ľ��

int compare_op(char op1, char op2) {
	static map<char, int> op2level = {
		{'+', 10},
		{'-', 10},
		{'*', 20},
		{'/', 20},
		{'=', 5},
		{'(', 0},
		{'[', 0},
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

void ExprTransformer::push_op(char op) {
	if (op == ')') {  // ���op��')',ֱ��'('��ջ,Ȼ��return
		
		while (true) { 
			char prev_op = op_stack_.back();
			op_stack_.pop_back();
			if (prev_op == '(') {
				return;
			}
			calulate(prev_op);

		}
		return;
	}

	if (op == ']') {  // ���op��']',ֱ��'['��ջ,����ȡ����Ԫ�����㣬Ȼ��return
		while (true) {
			char prev_op = op_stack_.back();
			op_stack_.pop_back();
			calulate(prev_op);
			if (prev_op == '[') { 
				return; 
			}
		}
	}

	if (op_stack_.empty() ||op == '(' || op == '[' || compare_op(op, op_stack_.back()) > 0) {
		// ��������op��'('��'[',ֱ����ջ
		// �������ȼ�����ջ��Ԫ������ջ��ע�⵱'('��'['Ϊջ��Ԫ��ʱ�����ȼ���С��
		op_stack_.push_back(op);
	} else {
		// ������ջֱ��ջ�����ȼ�С��op��������'('
		do {
			char prev_op = op_stack_.back();
			if (prev_op == '(' || prev_op == '[') { break; }
			op_stack_.pop_back();
			calulate(prev_op);
		} while ((!op_stack_.empty() && compare_op(op, op_stack_.back()) <= 0) );
		op_stack_.push_back(op);
	}
}


void ExprTransformer::push_value(shared_ptr<TableEntry> p_entry) {
	this->value_stack_.push_back(p_entry);
}

shared_ptr<TableEntry> ExprTransformer::pop() {
	while (!op_stack_.empty()) {
		char op = op_stack_.back();
		op_stack_.pop_back();
		calulate(op);
	}
	auto ret = value_stack_.back();
	this->clear();
	return ret;
}

void ExprTransformer::clear() {
	op_stack_.clear();
	value_stack_.clear();
	cnt_ = 0;
}

void ExprTransformer::calulate(char op) {
	shared_ptr<TableEntry> right = value_stack_.back();
	value_stack_.pop_back();
	shared_ptr<TableEntry> left =  value_stack_.back();
	value_stack_.pop_back();
	
	shared_ptr<Quaternion> quater;
	
	// �����Ƿ��Ǹ�ֵ���
	if (op == '=') {
		quater = QuaternionFactory::Assign(left, right);
		value_stack_.push_back(left);
		im_coder_.addQuater(quater);
		return;
	}
	
	string t_name = string("@t") + to_string(this->cnt_++);
	auto temp = make_shared<TempEntry>(t_name);
	switch (op) {
	case '+':
		//if (left == nullptr) { return; }
		if (left == nullptr) {
			// ����Ҫ�ѵ���opB��ѹջ��ȥ
			this->value_stack_.push_back(right);
			return;
		}
		quater = QuaternionFactory::Add(temp, left, right);
		break;
	case '-':
		if (left == nullptr) {
			quater = QuaternionFactory::Neg(temp, right);
		}else {
			quater = QuaternionFactory::Sub(temp, left, right);
		}
		break;
	case '*':
		quater = QuaternionFactory::Mul(temp, left, right);
		break;
	case '/':
		quater = QuaternionFactory::Div(temp, left, right);
		break;
	case '[':
		quater = QuaternionFactory::getArrayElem(temp, left, right);
		break;
	default:
		break;
	}
	// �����ջ
	value_stack_.push_back(temp);
	im_coder_.addQuater(quater);
}


