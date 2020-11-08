#include "ExprTransformer.h"

void ExprTransformer::push_op(char op) {
	if (op_stack_.empty() || compare_op(op, op_stack_.back()) > 0) {
		op_stack_.push_back(op);
	} else {
		// 反复出栈直至栈顶优先级小于op
		do {
			char prev_op = op_stack_.back();
			op_stack_.pop_back();
			calulate(prev_op);
		} while (!op_stack_.empty() && compare_op(op, op_stack_.back()) <= 0);
		op_stack_.push_back(op);
	}
}


void ExprTransformer::push_value(shared_ptr<TableEntry> p_entry) {
	this->value_stack_.push_back(p_entry);
	if (op_stack_.size() == 1 && value_stack_.size() == 1) {
		if (op_stack_[0] == '+') {
			op_stack_.pop_back();
		} else if (op_stack_[0] == '-') {

			char op = op_stack_.back();
			op_stack_.pop_back();
			shared_ptr<TableEntry> left = value_stack_.back();
			value_stack_.pop_back();
			auto temp = make_shared<TempEntry>(sym_table_.getUnsedName());
			shared_ptr<Quaternion> quater = QuaternionFactory::Neg(temp, left);
			value_stack_.push_back(temp);
			im_coder_.addQuater(quater);
		}
	}
}

shared_ptr<TableEntry> ExprTransformer::pop() {
	while (!op_stack_.empty()) {
		char op = op_stack_.back();
		op_stack_.pop_back();
		calulate(op);
	}
	return value_stack_.back();
}

void ExprTransformer::calulate(char op) {
	shared_ptr<TableEntry> right = value_stack_.back();
	value_stack_.pop_back();
	shared_ptr<TableEntry> left = value_stack_.back();
	value_stack_.pop_back();

	auto temp = make_shared<TempEntry>(sym_table_.getUnsedName());
	shared_ptr<Quaternion> quater;
	switch (op) {
	case '+':
		quater = QuaternionFactory::Add(temp, left, right);
		break;
	case '-':
		quater = QuaternionFactory::Sub(temp, left, right);
		break;
	case '*':
		quater = QuaternionFactory::Mul(temp, left, right);
		break;
	case '/':
		quater = QuaternionFactory::Div(temp, left, right);
		break;
	default:
		break;
	}
	// 结果入栈
	value_stack_.push_back(temp);
	im_coder_.addQuater(quater);
}
