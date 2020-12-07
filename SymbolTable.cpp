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
ConstEntry::ConstEntry(ValueType entry_value_type, string identifier, string value):
	TableEntry(EntryType::CONST, entry_value_type, identifier)  {

	if (entry_value_type == ValueType::CHARV) {
		value = "\'" + value + "\'";
	}
	value_ = value;
}

void ConstEntry::setValue(string value) {
	if (this->value_type() == ValueType::CHARV) {
		value = "\'" + value + "\'";
	}
	value_ = value;
}



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
* 检查形参与实参的类型是否匹配
* 由于一行只会出现最多一个错误，则先检查参数个数是否一致，如果不一致，则直接跳过对类型一致性的检查
* 换言之，进行此检查的前提是形参实参的个数一致
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

bool VarEntry::isArray() {
	return shape_.size() > 0;
}

int VarEntry::ByteSize() {
	int cnt = 1;
	for (int x : this->shape_) {
		cnt *= x;
	}
	if (this->value_type() == ValueType::INTV) {
		return cnt * 4;
	} 
	return cnt;
}

int VarEntry::getDimByte(int i) {
	int cnt;
	if (shape_.size() == 2 && i == 0) {
		cnt = shape_[1];
	} else {
		cnt = 1;
	}
	if (this->value_type() == ValueType::INTV) {
		return cnt * 4;
	}
	return cnt;

}

/******************************************** ImmediateEntry ********************************************************/

int ImmediateEntry::str_cnt = 0;

ImmediateEntry::ImmediateEntry(ValueType value_type, string value)
	: TableEntry(EntryType::IMMEDIATE, value_type, "")
{	
	if (value_type == ValueType::STRINGV) {
		str_name = string(".str" + to_string(++str_cnt));
	}
	if (this->value_type() == ValueType::CHARV) {
		value =  "\'" + value + "\'";
	}
	value_.push_back(value);

}

ImmediateEntry::ImmediateEntry(ValueType value_type, vector<int>& shape, vector<string>& value) :
	TableEntry(EntryType::IMMEDIATE, value_type, ""), shape_(shape)
{
	if (value_type == ValueType::STRINGV) {
		str_name = string(".str" + to_string(++str_cnt));
	}
	if (this->value_type() == ValueType::CHARV) {
		for (string& x : value) {
			x = "\'" + x + "\'";
		}
	}
	value_ = value;
}

/**
* 单值立即数可能会在表达式中用到，将它的字符常量视为identifier
*/
string& ImmediateEntry::identifier() {
	assert(this->shape_.empty());
	if (this->value_type() == ValueType::STRINGV) {
		return str_name;
	}
	return value_[0];
}

string ImmediateEntry::getValue() {
	assert(this->shape_.empty());
	return value_[0];
}

const vector<string>& ImmediateEntry::initializingList() {
	return this->value_;
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
* 
* 1.如果在作用域内有重名错误,返回False,否则返回true
* 2.为了即使出现错误仍能继续编译，无论是否出现重名错误，都要将输入项添加到符号表中
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


/*重定位操作： 编译完当前函数后删除无效部分; 将作用域指针指向全局变量*/
void SymbolTable::reset() {
	stack_table_.erase(stack_table_.begin() + curr_block_head, stack_table_.end());
	curr_block_head = 0; 
}


/* 在作用域内未定义过，返回true; 否则返回False */
bool SymbolTable::checkDefine(string& sym) {
	assert(sym != "");
	// 检查重名错误只在当前作用域查找
	for (int i = stack_table_.size() - 1; i >= curr_block_head; i--) {
		if (strcmp_wo_case(stack_table_[i]->identifier(),sym)) {
			return false;
		}
	}
	return true;

}


/*检查元素在全局可见范围内是否定义过*/
shared_ptr<TableEntry> SymbolTable::checkReference(string identifier) {
	assert(identifier != "");
	// 检查元素是否定义过需要在当前和全局作用域都查找，也就是整个符号表
	// 逆序查找，先找局部作用域，再找全局作用域
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

