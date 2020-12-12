#ifndef REGISTER_POOL_H_
#define REGISTER_POOL_H_

#include<map>
#include<memory>
#include"SymbolTable.h"
#include<set>

class OptMips::OptMipsFunctionGenerator;

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

/** t0-t9�Ĵ����д���У�
* 1.����(��Ҫ����ʱ����)
* 2.������������(��ImmedEntry��ʾ,���Ƿ�����ڼĴ���ʱ�Ƚ�value�Ƿ����)
* 3.����ֵ
*/

class RegisterPool {

public:
	OptMips::OptMipsFunctionGenerator* func_generator_; 
	RegisterPool();

	// �����ʱ�Ĵ����أ�����Ծ����ʱ������д���ڴ�;������ֱ�����
	void clearTempRegs(set<TableEntry*> active_set);

	/** assign an available $s0-$s7 reg for var in memory space, return reg name
	* load_var: ���������ڼĴ�����ʱ���Ƿ���Ҫ���ڴ���load
	 * 1. �������ĳ���Ĵ�������ľ��Ǹñ�����ֵ�����ؼĴ�����ţ��Ĵ�����������
	 * 2. ��������ڣ����п���Ĵ�����ȡһ������Ĵ����� load�������Ĵ�����������
	 * 3. ��������ڿ��мĴ�����ȡ����β�Ĵ������Ƚ��Ĵ����ڵ�ֵд���ڴ棬��load�±�������������
	*/
	string assign_temp_reg(shared_ptr<TableEntry> var, bool load_var = true);
	/**assign an available $t0-$t9 reg for const, immediate num; return reg name*/
	string assign_temp_reg(string value);
	/* ����null��ʾ��ֵ�ǻ���ֵ�����豣�档����ֱ�ӱ��ٷ��� */
	string assign_temp_reg();

private:
	bool hasEmptyTReg(int* p_index);
	//bool hasEmptySReg(int* p_index);
	// find if the var has been map to a regiter
	bool find(shared_ptr<TableEntry> var, int* p_index);

	shared_ptr<TableEntry> t_regs[T_NUM];
	//shared_ptr<TableEntry> s_regs[S_NUM];
	//shared_ptr<TableEntry> v_regs[V_NUM];

	vector<int> lru_queue_;

};

inline string treg_name(int i) { return "$t" + to_string(i); }

#endif // !REGISTER_POOL_H_


