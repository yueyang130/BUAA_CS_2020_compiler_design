#ifndef TOOLS_H_
#define TOOLS_H_

#include <string>
#include <algorithm>
#include"SymbolTable.h"

using namespace std;

/*
不区分大小写，判断两个string类是否相等
*/
bool strcmp_wo_case(string str1, string str2);

/*用于常数合并*/
bool const_or_immed(TableEntry* entry);
int getValue(TableEntry* entry);
int calValue(symbolType alu_type, TableEntry* opA, TableEntry* opB);

#endif // !TOOLS_H_

