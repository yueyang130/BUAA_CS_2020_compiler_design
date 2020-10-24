#include "SymbolTable.h"

TableEntry::TableEntry(EntryType entry_type, ValueType entry_value_type, string identifier) {
	this->entry_type_ = entry_type;
	this->entry_value_type_ = entry_value_type;
	this->identifier_ = identifier;
}

/************************************* method definition for SymbolTable *************************************************/
SymbolTable::SymbolTable(int init_capability = 4096) {
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
* �������������,����False,���򷵻�true
* Ϊ�˼�ʹ���ִ������ܼ������룬�����Ƿ�����������󣬶�Ҫ����������ӵ����ű���
*/
bool SymbolTable::add(TableEntry entry) {
	stack_table_.push_back(entry);
	// �����������ֻ�ڵ�ǰ���������
	for (int i = stack_table_.size() - 2; i >= curr_block_head; i--) {
		if (stack_table_[i].identifier() == entry.identifier()) {
			return false;
		}
	}
	return true;
}

/*�ض�λ������ �����굱ǰ������ɾ����Ч����; ��������ָ��ָ��ȫ�ֱ���*/
void SymbolTable::reset() {
	stack_table_.erase(stack_table_.begin() + curr_block_head, stack_table_.end());
	curr_block_head = 0; 
}

/* ����δ�ض��壬����true; ���򷵻�False */
bool SymbolTable::checkRedefine(string& sym) {
	// �����������ֻ�ڵ�ǰ���������
	for (int i = stack_table_.size() - 1; i >= curr_block_head; i--) {
		if (stack_table_[i].identifier() == sym) {
			return false;
		}
	}
	return true;

}

/*���Ԫ���Ƿ����*/
TableEntry* SymbolTable::find(string identifier) {
	// ���Ԫ���Ƿ������Ҫ�ڵ�ǰ��ȫ�������򶼲��ң�Ҳ�����������ű�
	// ������ң����Ҿֲ�����������ȫ��������
	for (auto top = stack_table_.rbegin(), bottom = stack_table_.rend(); top != bottom; top++) {
		if (top->identifier() == identifier) {
			return &(*top);
		}
	}
	return NULL;
}

/**
* ����β���ʵ�ε������Ƿ�ƥ��
* ����һ��ֻ��������һ���������ȼ����������Ƿ�һ�£������һ�£���ֱ������������һ���Եļ��
* ����֮�����д˼���ǰ�����β�ʵ�εĸ���һ��
*/
bool FunctionEntry::checkParamList(vector<ValueType>& actual_param_list) {
	int num = actual_param_list.size();
	for (int i = 0; i < num; i++) {
		if (actual_param_list[i] != formal_param_list_[i]) {
			return false;
		}
	}
	return true;
}
