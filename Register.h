#ifndef REGISTER_H_
#define REGISTER_H_

#include<map>
#include<memory>
#include"SymbolTable.h"

const int A_NUM = 4;
const int T_NUM = 10;
const int S_NUM = 8;
const int V_NUM = 2;


enum RegType {
	A_REG,
	T_REG,
	S_REG,
	V_REG
};

/*
��MIPS�У��ֲ�������ֵ��Ҫ���ڼĴ����У����ڴ�ռ��е�ֵ��һ����ȷ��
ֻ���ڼĴ�����û������ֲ�����ʱ��˵������ȷ��ֵд���˼Ĵ�����
*/

class Register {


public:
	Register(vector<string>& mips_list);
	bool find(shared_ptr<TableEntry> var, string* p_str);
	bool hasEmptyGbReg(int* p_index);
	bool hasEmptyTpReg(int* p_index);
	/*assign $s0-$s7 reg for var in memory space, return reg name*/
	string assign_global_reg(shared_ptr<TableEntry> var, map<shared_ptr<TableEntry>, int>& var_offset_map);
	/*assign $t0-$t9 reg for temp(in memory space) or const, immediate num; return reg name*/
	string assign_temp_reg(shared_ptr<TableEntry> var, map<shared_ptr<TableEntry>, int>& var_offset_map);
	// ��var, temp, const, immediate num�����ض��ļĴ���
	string assign_value_to_reg(shared_ptr<TableEntry> entry, RegType type, int index);
	// ��ָ���Ĵ�����ֵ��������
	void assign_ret_to_var(shared_ptr<TableEntry> entry, RegType type, int index);

private:
	vector<string>& mips_list;

	shared_ptr<TableEntry> a_regs[A_NUM];
	shared_ptr<TableEntry> t_regs[T_NUM];
	shared_ptr<TableEntry> s_regs[S_NUM];
	shared_ptr<TableEntry> v_regs[V_NUM];

};

extern Register* myReg;

#endif // !REGISTER_H_


