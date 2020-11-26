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
	/*��������������*/
	/*���������壾   ::=   int����ʶ��������������{,����ʶ��������������}  
                  | char����ʶ���������ַ���{,����ʶ���������ַ���}  
	*/
	ConstEntry(ValueType entry_value_type, string identifier, string value = "");
		
	
	void setValue(string value);
	virtual string getValue() { return value_;  }
private:
	string value_;
};


/*��ͬ��constEntry��¼��value�������ĳ�ʼ��������ImmediateEntry*/
class VarEntry : public TableEntry {
public:
	/*���������Ĺ��캯��*/
	VarEntry(ValueType entry_value_type, string identifier) :
		TableEntry(EntryType::VAR, entry_value_type, identifier) {};
	/*��������Ĺ��캯��*/
	VarEntry(ValueType entry_value_type, string identifier, vector<int>& shape) :
		TableEntry(EntryType::VAR, entry_value_type, identifier),shape_(shape)  {};

	bool checkSize(vector<int> assign_shape);
	bool isArray();
	/*���ڴ�ռ���Ҫռ�ö����ֽڣ� ��������͵������������õķ���*/
	int ByteSize();
	// ����ĵ�iά��һ��Ԫ����Ҫռ�ÿռ�Ĵ�С
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
	/*��ֵ�������Ĺ��캯��*/
	ImmediateEntry(ValueType value_type, string value);
	/*�����������Ĺ��캯��*/
	ImmediateEntry(ValueType value_type, vector<int>& shape, vector<string>& value);
	/*
	����int��char,���س�ֵ���ַ�����ʽ; 
	����str,����#str{str_cnt};
	����Array�����Դ���
	*/
	virtual string& identifier();
	virtual string getValue();
	/*�������ڱ�����ʼ��ʱ�Ļ������б�*/
	const vector<string>& initializingList();

private:
	static int str_cnt;
	string str_name;
	vector<int> shape_;
	vector<string> value_;
};



/*��ʱ������*/
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
	void set();											// ��λ����
	bool add(shared_ptr<TableEntry> p_entry);			// ��ӱ���
	void reset();										// �ض�λ����
	bool checkDefine(string& sym);						// ��������ض���
	shared_ptr<TableEntry> checkReference(string identifier);		// ��������ʱ�Ƿ��Ѷ���˱�ʶ��
	string getUnsedName();  // ����һ���ڵ�ǰ������û��ʹ�ù��ı���������Ϊ��ʱ����������

private:
	SymbolTable(int init_capability = 4096);
	vector<shared_ptr<TableEntry>> stack_table_; // ջʽ�б�;
	int curr_block_head; // ָ��ǰ������ĵ�һ��Ԫ��
};


#endif // ! SYMBOL_TABLE_H_




