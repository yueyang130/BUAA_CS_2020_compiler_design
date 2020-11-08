#include "SymbolTable.h"
#include "tools.h"
#include "assert.h"
#include<sstream>


/******************************************** TableEntry ********************************************************/

TableEntry::TableEntry(EntryType entry_type, ValueType entry_value_type, string identifier) {
	this->entry_type_ = entry_type;
	this->entry_value_type_ = entry_value_type;
	this->identifier_ = identifier;
}

/******************************************** ConstEntry ********************************************************/
//string ConstEntry::getValue() {
//	stringstream ss;
//	if (this->shape_.empty()) {
//		return this->value_[0];
//	}
//	else if (this->shape_.size() == 1) {
//	}
//	else if (this->shape_.size() == 2) {
//		int d1 = this->shape_[0];
//		int d2 = this->shape_[1];
//		for (int i = 0; i < d1; i++) {
//			ss << "{";
//
//		}
//
//	} else {
//		ss << "error dimension of the const array";
//	}
//
//}


/******************************************** FunctionEntry ******************************************************/

/**
* ����β���ʵ�ε������Ƿ�ƥ��
* ����һ��ֻ��������һ���������ȼ����������Ƿ�һ�£������һ�£���ֱ������������һ���Եļ��
* ����֮�����д˼���ǰ�����β�ʵ�εĸ���һ��
*/
bool FunctionEntry::checkParamList(vector<ValueType>& actual_param_list) {
	
	for (int i = 0; i < actual_param_list.size(); i++) {
		if (actual_param_list[i] != ValueType::UNKNOWN && actual_param_list[i] != formal_param_list_[i]) {
			return false;
		}
	}
	return true;
}

/******************************************** VarEntry *************************************************************/

bool VarEntry::checkSize(vector<int> assign_shape) {
	return assign_shape == shape_;
}

/******************************************** ImmediateEntry ********************************************************/

/**
* ��ֵ���������ܻ��ڱ��ʽ���õ����������ַ�������Ϊidentifier
*/
string& ImmediateEntry::identifier() {
	assert(this->shape_.empty());
	return value_[0];
}

//int ImmediateEntry::getIntValue() {
//	assert(value_type() == ValueType::INTV);
//	return stoi(value_);
//}
//
//char ImmediateEntry::getCharValue() {
//	assert(value_type() == ValueType::CHARV);
//	return value_[0];
//}
//




/************************************* method definition for SymbolTable *************************************************/
SymbolTable::SymbolTable(int init_capability) {
	stack_table_.reserve(init_capability);
	curr_block_head = 0;
}

SymbolTable& SymbolTable::getInstance() {
	// ����֣��ڴ�single-instance-pattern�У�����Ĭ�Ϲ��캯�������������instan()�ᱨ��
	static SymbolTable instan;
	return instan;
}

/*��λ�������������ڵ�ǰ������ķ��ű�*/
void SymbolTable::set() {
	curr_block_head = stack_table_.size();
}


/**
* ��ӷ��ű���
* 
* 1.�����������������������,����False,���򷵻�true
* 2.Ϊ�˼�ʹ���ִ������ܼ������룬�����Ƿ�����������󣬶�Ҫ����������ӵ����ű���
*/
bool SymbolTable::add(shared_ptr<TableEntry> p_entry) {
	EntryType type = p_entry->entry_type();
	bool success_add = true;
	if (type == EntryType::CONST || type == EntryType::FUNCTION || type == EntryType::VAR) {
		success_add = checkDefine(p_entry->identifier());
	}
	stack_table_.push_back(p_entry);
	return success_add;
}


/*�ض�λ������ �����굱ǰ������ɾ����Ч����; ��������ָ��ָ��ȫ�ֱ���*/
void SymbolTable::reset() {
	stack_table_.erase(stack_table_.begin() + curr_block_head, stack_table_.end());
	curr_block_head = 0; 
}


/* ����������δ�����������true; ���򷵻�False */
bool SymbolTable::checkDefine(string& sym) {
	assert(sym != "");
	// �����������ֻ�ڵ�ǰ���������
	for (int i = stack_table_.size() - 1; i >= curr_block_head; i--) {
		if (strcmp_wo_case(stack_table_[i]->identifier(),sym)) {
			return false;
		}
	}
	return true;

}


/*���Ԫ����ȫ�ֿɼ���Χ���Ƿ����*/
shared_ptr<TableEntry> SymbolTable::checkReference(string identifier) {
	assert(identifier != "");
	// ���Ԫ���Ƿ������Ҫ�ڵ�ǰ��ȫ�������򶼲��ң�Ҳ�����������ű�
	// ������ң����Ҿֲ�����������ȫ��������
	for (auto top = stack_table_.rbegin(), bottom = stack_table_.rend(); top != bottom; top++) {
		if (strcmp_wo_case((*top)->identifier(), identifier)) {
			return *top;
		}
	}
	return shared_ptr<TableEntry>();
}

string SymbolTable::getUnsedName() {
	for (int i = 0; ; i++) {
		string name = string("i") + to_string(i);
		if (checkDefine(name)) {
			return name;
		}
	}
}
