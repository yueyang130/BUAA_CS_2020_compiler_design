#ifndef SYMBOL_TABLE_H_
#define SYMBOL_TABLE_H_

#include "type.h"
#include <vector>
#include<string>
#include<memory>
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
	/*
	dynamic_cast can only be used in the case of a pointer or reference cast,
	and in addition to the compile time check, it does an additional run time check that the cast is legal.
	It requires that the class in question have at least 1 virtual method,
	which allows the compiler (if it supports RTTI) to perform this additional check.
	*/
	virtual ~TableEntry() {};

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

	VarEntry(ValueType entry_value_type, string identifier, vector<int>& shape) :
		TableEntry(EntryType::VAR, entry_value_type, identifier),shape_(shape)  {};

	bool checkSize(vector<int> assign_shape);
private:
	vector<int> shape_;

};

class FunctionEntry : public TableEntry {
public:
	FunctionEntry(ValueType entry_value_type, string identifier, vector<ValueType>& formal_param_list) :
		TableEntry(EntryType::FUNCTION, entry_value_type, identifier) {
		formal_param_list_ = formal_param_list;
	};

	FunctionEntry(ValueType entry_value_type, string identifier) :
		TableEntry(EntryType::FUNCTION, entry_value_type, identifier) {};

	inline int formal_param_num() { return formal_param_list_.size(); }
	inline void setParamList(vector<ValueType>& formal_param_list) { this->formal_param_list_ = formal_param_list;  }
	bool checkParamList(vector<ValueType>& actual_param_list);

private:
	vector<ValueType> formal_param_list_;

};

class SymbolTable {
public:
	static SymbolTable& getInstance();
	void set();											// 定位操作
	bool add(shared_ptr<TableEntry> p_entry);			// 添加表项
	void reset();										// 重定位操作
	bool checkDefine(string& sym);						// 检查名字重定义
	shared_ptr<TableEntry> checkReference(string identifier);		// 查找引用时是否已定义此标识符

private:
	SymbolTable(int init_capability = 4096);
	vector<shared_ptr<TableEntry>> stack_table_; // 栈式列表;
	int curr_block_head; // 指向当前作用域的第一个元素
};


#endif // ! SYMBOL_TABLE_H_




