#ifndef SYMBOL_TABLE_H_
#define SYMBOL_TABLE_H_

#include "type.h"
#include <vector>
using namespace std;

class TableEntry {

private:
	EntryType entry_type_;
	ValueType entry_value_type_;
	string identifier_;

public:
	TableEntry(EntryType entry_type, ValueType entry_value_type, string identifier);
	EntryType entry_type() { return entry_type_; }
	ValueType entry_value_type() { return entry_value_type_; }
	string& identifier() { return identifier_; }

};

class ConstEntry : public TableEntry {
public:
	ConstEntry(ValueType entry_value_type, string identifier) :
		TableEntry(EntryType::CONST, entry_value_type, identifier) {};

};

class VarEntry : public TableEntry {
public:
	VarEntry(ValueType entry_value_type, string identifier) :
		TableEntry(EntryType::VAR, entry_value_type, identifier) {};
};

class FunctionEntry : public TableEntry {
public:
	FunctionEntry(ValueType entry_value_type, string identifier, vector<ValueType>& formal_param_list) :
		TableEntry(EntryType::FUNCTION, entry_value_type, identifier), formal_param_list_(formal_param_list) {};
	inline int formal_param_num() { return formal_param_list_.size(); }
	bool checkParamList(vector<ValueType>& actual_param_list);

private:
	vector<ValueType> formal_param_list_;

};

class SymbolTable {
public:
	static SymbolTable& getInstance();
	void set();							// 定位操作
	bool add(TableEntry entry);			// 添加表项
	void reset();						// 重定位操作
	bool checkRedefine(string& sym);    // 检查名字重定义
	TableEntry* find(string identifier); // 查找引用时是否已定义此标识符

private:
	SymbolTable(int init_capability);
	vector<TableEntry> stack_table_; // 栈式列表;
	vector<TableEntry>::size_type curr_block_head; // 指向当前作用域的第一个元素
};


#endif // ! SYMBOL_TABLE_H_




