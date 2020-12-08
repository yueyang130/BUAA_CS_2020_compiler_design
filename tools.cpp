#include "tools.h"

/**
* ��Ϊstr1��str2�ǰ�ֵ���ݣ����Կ���ֱ���ڲ����ϲ���
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
	return stoi(entry->getValue().c_str());
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
