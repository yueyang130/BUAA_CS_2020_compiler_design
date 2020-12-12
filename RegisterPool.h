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
在MIPS中，局部变量的值主要是在寄存器中，在内存空间中的值不一定正确。
只有在寄存器中没有这个局部变量时，说明把正确的值写回了寄存器。
*/

/** t0-t9寄存器中存放有：
* 1.变量(主要是临时变量)
* 2.立即数，常数(用ImmedEntry表示,看是否存在于寄存器时比较value是否相等)
* 3.缓存值
*/

class RegisterPool {

public:
	OptMips::OptMipsFunctionGenerator* func_generator_; 
	RegisterPool();

	// 清空临时寄存器池，将活跃的临时变量回写到内存;立即数直接清除
	void clearTempRegs(set<TableEntry*> active_set);

	/** assign an available $s0-$s7 reg for var in memory space, return reg name
	* load_var: 当变量不在寄存器中时，是否需要从内存中load
	 * 1. 如果存在某个寄存器保存的就是该变量的值，返回寄存器编号，寄存器移至队首
	 * 2. 如果不存在，且有空余寄存器，取一个空余寄存器， load变量，寄存器移至队首
	 * 3. 如果不存在空闲寄存器，取出队尾寄存器，先将寄存器内的值写回内存，再load新变量，移至队首
	*/
	string assign_temp_reg(shared_ptr<TableEntry> var, bool load_var = true);
	/**assign an available $t0-$t9 reg for const, immediate num; return reg name*/
	string assign_temp_reg(string value);
	/* 传入null表示该值是缓存值，无需保存。可以直接被再分配 */
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


