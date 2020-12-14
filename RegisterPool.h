#ifndef REGISTER_POOL_H_
#define REGISTER_POOL_H_

#include<map>
#include<memory>
#include"SymbolTable.h"
#include<unordered_set>
#include<unordered_map>

// 不包括$a0
//const int A_NUM = 3;
//const int V_NUM = 2;
const int T_NUM = 8;
const int S_NUM = 8 + 3 + 2;


// 两个缓存寄存器
const string buf_reg1 = "$t8";
const string buf_reg2 = "$t9";


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

/** t0-t7寄存器中存放有：
* 1.变量(主要是临时变量)
* 
* t8-t9存放:
* 2.立即数，常数(用ImmedEntry表示,看是否存在于寄存器时比较value是否相等)
* 3.缓存值
*/


namespace OptMips {
	class OptMipsFunctionGenerator;

	class RegisterPool {

	public:
		OptMips::OptMipsFunctionGenerator* func_generator_;
		RegisterPool();

		void count_var_ref(unordered_map<shared_ptr<TableEntry>, int> ref_map);
		void deal_var_decalre(shared_ptr<TableEntry> var, string value, int offset);
		void load_gb_var();
		void clear_sreg();

		// 清空临时寄存器池，将活跃的临时变量回写到内存;立即数直接清除
		void treg_write_back(unordered_set<shared_ptr<TableEntry>> active_set);
		void clearTempRegs();

		// 函数调用时，保存现场
		void save_tregs(vector<string>& save_list);
		void restore_tregs(vector<string>& save_list);

		/** assign an available $s0-$s7 reg for var in memory space, return reg name
		* load_var: 当变量不在寄存器中时，是否需要从内存中load
		 * 1. 如果存在某个寄存器保存的就是该变量的值，返回寄存器编号，寄存器移至队首
		 * 2. 如果不存在，且有空余寄存器，取一个空余寄存器， load变量，寄存器移至队首
		 * 3. 如果不存在空闲寄存器，取出队尾寄存器，先将寄存器内的值写回内存，再load新变量，移至队首
		*/
		string assign_reg(shared_ptr<TableEntry> var, bool load_var = true);
		vector<string> assign_reg(shared_ptr<TableEntry> var1, shared_ptr<TableEntry> var2, bool load_var = true);
		/**assign an available $t0-$t9 reg for const, immediate num; return reg name*/
		//string assign_buf_reg(string value);
		///* 传入null表示该值是缓存值，无需保存。可以直接被再分配 */
		//string assign_buf_reg();

	private:
		bool hasEmptyTReg(int* p_index);
		//bool hasEmptySReg(int* p_index);
		// find if the var has been map to a regiter
		bool find_var_in_treg(shared_ptr<TableEntry> var, int* p_index);

		bool find_var_in_sreg(shared_ptr<TableEntry> var, int* p_index);

		//bool find(string value, int* p_index);

		shared_ptr<TableEntry> t_regs[T_NUM];
		shared_ptr<TableEntry> s_regs[S_NUM];

		vector<int> lru_queue_;


		string assign_temp_reg(shared_ptr<TableEntry> var, bool load_var = true);
		string assign_s_reg(shared_ptr<TableEntry> var, bool load_var = true);

	};

	inline string treg_name(int i) { 
		return "$t" + to_string(i); 
	}

	inline string bufreg_name(int i) { return "$t" + to_string(i + 8); }

	inline string sreg_name(int i) {
		if (i < 8)
			return "$s" + to_string(i);
		if (i < 11)
			return "$a" + to_string(i - 7);
		// i < 13
		return "$v" + to_string(i - 11);
	}
}

#endif // !REGISTER_POOL_H_


