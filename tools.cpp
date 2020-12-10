#include "tools.h"

/**
* 因为str1和str2是按值传递，所以可以直接在参数上操作
*/
bool strcmp_wo_case(string str1, string str2) {
	transform(str1.begin(), str1.end(), str1.begin(), ::tolower);
	transform(str2.begin(), str2.end(), str2.begin(), ::tolower);
	return str1 == str2;
}

bool const_or_immed(TableEntry* entry) {
	return entry->entry_type() == EntryType::CONST || entry->entry_type() == EntryType::IMMEDIATE;
}

int getValue(TableEntry* entry) {
	string v = entry->getValue();
	if (entry->value_type() == ValueType::INTV)
		return stoi(v.c_str());
	else {
		
		return v[1];
	}
		
}



int calValue(symbolType alu_type,TableEntry* opA, TableEntry* opB) {
	int a = getValue(opA), b = getValue(opB);
	switch (alu_type) {
	case symbolType::PLUS:
		return a + b;
	case symbolType::MINU:
		return a - b;
	case symbolType::MULT:
		return a * b;
	case symbolType::DIV:
		return a / b;
	default:
		return 0;
	}
}

string new_label() { 
	static int label_cnt = 0;
	return "label" + to_string(++label_cnt); 
}



int log2(int value)   //非递归判断一个数是2的多少次方
{
	int x = 0;
	while (value > 1) {
		value >>= 1;
		x++;
	}
	return x;
}
