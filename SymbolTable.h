#ifndef SYMBOL_TABLE_H_
#define SYMBOL_TABLE_H_

#include "type.h"
#include <vector>
#include<string>
#include<memory>
using namespace std;

/**
* TableEntry	Const		Var		Function		Label		immediate(int/char/string)
*/

class TableEntry {

private:
	EntryType entry_type_;
	ValueType entry_value_type_;
	string identifier_;

public:
	TableEntry(EntryType entry_type, ValueType entry_value_type, string identifier="");
	EntryType entry_type() { return entry_type_; }
	ValueType value_type() { return entry_value_type_; }
	virtual string& identifier() { return identifier_; }
	virtual string getValue() { return ""; }
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
	/*常量不能是数组*/
	/*＜常量定义＞   ::=   int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞}  
                  | char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}  
	*/
	ConstEntry(ValueType entry_value_type, string identifier, string value = "");
		
	
	void setValue(string value);
	virtual string getValue() { return value_;  }
private:
	string value_;
};


/*不同于constEntry记录了value，变量的初始化依赖于ImmediateEntry*/
class VarEntry : public TableEntry {
public:
	/*单个变量的构造函数*/
	VarEntry(ValueType entry_value_type, string identifier) :
		TableEntry(EntryType::VAR, entry_value_type, identifier) {};
	/*数组变量的构造函数*/
	VarEntry(ValueType entry_value_type, string identifier, vector<int>& shape) :
		TableEntry(EntryType::VAR, entry_value_type, identifier),shape_(shape)  {};

	bool checkSize(vector<int> assign_shape);
	bool isArray();
	/*在内存空间中要占用多少字节； 对于数组和单个变量都适用的方法*/
	int ByteSize();
	// 数组的第i维的一个元素需要占用空间的大小
	int getP(int i);
	//vector<int> shape() { return shape_;  }
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


class LabelEntry : public TableEntry {
public:
	LabelEntry(string label_name) :
		TableEntry(EntryType::LABEL, (ValueType)NULL, label_name) {};

private:

};


/*ImmediateEntry*/

class ImmediateEntry : public TableEntry{
public:
	/*单值立即数的构造函数*/
	ImmediateEntry(ValueType value_type, string value);
	/*数组立即数的构造函数*/
	ImmediateEntry(ValueType value_type, vector<int>& shape, vector<string>& value);
	/*
	对于int和char,返回常值的字符串形式; 
	对于str,返回#str{str_cnt};
	对于Array，断言错误
	*/
	virtual string& identifier();
	virtual string getValue();
	/*返回用于变量初始化时的花括号列表*/
	const vector<string>& initializingList();

private:
	static int str_cnt;
	string str_name;
	vector<int> shape_;
	vector<string> value_;
};



/*临时变量项*/
class TempEntry : public TableEntry {
public:
	TempEntry(string identifier, ValueType value_type = ValueType::INTV) :
		TableEntry(EntryType::TEMP, value_type, identifier) {
	};

private:

};


class SymbolTable {
public:
	static SymbolTable& getInstance();
	void set();											// 定位操作
	bool add(shared_ptr<TableEntry> p_entry);			// 添加表项
	void reset();										// 重定位操作
	bool checkDefine(string& sym);						// 检查名字重定义
	shared_ptr<TableEntry> checkReference(string identifier);		// 查找引用时是否已定义此标识符
	string getUnsedName();  // 返回一个在当前定义域没有使用过的变量名，作为临时变量的名字

private:
	SymbolTable(int init_capability = 4096);
	vector<shared_ptr<TableEntry>> stack_table_; // 栈式列表;
	int curr_block_head; // 指向当前作用域的第一个元素
};


#endif // ! SYMBOL_TABLE_H_




