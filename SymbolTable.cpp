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
	// 很奇怪，在此single-instance-pattern中，调用默认构造函数，如果带括号instan()会报错。
	static SymbolTable instan;
	return instan;
}

/*定位操作：建立属于当前作用域的符号表*/
void SymbolTable::set() {
	curr_block_head = stack_table_.size();
}

/**
* 添加符号表项
* 如果有重名错误,返回False,否则返回true
* 为了即使出现错误仍能继续编译，无论是否出现重名错误，都要将输入项添加到符号表中
*/
bool SymbolTable::add(TableEntry entry) {
	stack_table_.push_back(entry);
	// 检查重名错误只在当前作用域查找
	for (int i = stack_table_.size() - 2; i >= curr_block_head; i--) {
		if (stack_table_[i].identifier() == entry.identifier()) {
			return false;
		}
	}
	return true;
}

/*重定位操作： 编译完当前函数后删除无效部分; 将作用域指针指向全局变量*/
void SymbolTable::reset() {
	stack_table_.erase(stack_table_.begin() + curr_block_head, stack_table_.end());
	curr_block_head = 0; 
}

/* 名字未重定义，返回true; 否则返回False */
bool SymbolTable::checkRedefine(string& sym) {
	// 检查重名错误只在当前作用域查找
	for (int i = stack_table_.size() - 1; i >= curr_block_head; i--) {
		if (stack_table_[i].identifier() == sym) {
			return false;
		}
	}
	return true;

}

/*检查元素是否定义过*/
TableEntry* SymbolTable::find(string identifier) {
	// 检查元素是否定义过需要在当前和全局作用域都查找，也就是整个符号表
	// 逆序查找，先找局部作用域，再找全局作用域
	for (auto top = stack_table_.rbegin(), bottom = stack_table_.rend(); top != bottom; top++) {
		if (top->identifier() == identifier) {
			return &(*top);
		}
	}
	return NULL;
}

/**
* 检查形参与实参的类型是否匹配
* 由于一行只会出现最多一个错误，则先检查参数个数是否一致，如果不一致，则直接跳过对类型一致性的检查
* 换言之，进行此检查的前提是形参实参的个数一致
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
