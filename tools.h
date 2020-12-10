#ifndef TOOLS_H_
#define TOOLS_H_

#include <string>
#include <algorithm>
#include"SymbolTable.h"

using namespace std;

/*
�����ִ�Сд���ж�����string���Ƿ����
*/
bool strcmp_wo_case(string str1, string str2);

/*���ڳ����ϲ�*/
bool const_or_immed(TableEntry* entry);
int getValue(TableEntry* entry);
int calValue(symbolType alu_type, TableEntry* opA, TableEntry* opB);

/*���ɱ�ǩ����*/
string new_label();


/*ָ��ѡ��*/
int log2(int value);

#endif // !TOOLS_H_

