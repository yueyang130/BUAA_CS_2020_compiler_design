#ifndef REGISTER_POOL_H_
#define REGISTER_POOL_H_

#include<map>
#include<memory>
#include"SymbolTable.h"
#include<unordered_set>
#include<unordered_map>

// ������$a0
//const int A_NUM = 3;
//const int V_NUM = 2;
const int T_NUM = 8;
const int S_NUM = 8 + 3 + 2;


// ��������Ĵ���
const string buf_reg1 = "$t8";
const string buf_reg2 = "$t9";


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

/** t0-t7�Ĵ����д���У�
* 1.����(��Ҫ����ʱ����)
* 
* t8-t9���:
* 2.������������(��ImmedEntry��ʾ,���Ƿ�����ڼĴ���ʱ�Ƚ�value�Ƿ����)
* 3.����ֵ
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

		// �����ʱ�Ĵ����أ�����Ծ����ʱ������д���ڴ�;������ֱ�����
		void treg_write_back(unordered_set<shared_ptr<TableEntry>> active_set);
		void clearTempRegs();

		// ��������ʱ�������ֳ�
		void save_tregs(vector<string>& save_list);
		void restore_tregs(vector<string>& save_list);

		/** assign an available $s0-$s7 reg for var in memory space, return reg name
		* load_var: ���������ڼĴ�����ʱ���Ƿ���Ҫ���ڴ���load
		 * 1. �������ĳ���Ĵ�������ľ��Ǹñ�����ֵ�����ؼĴ�����ţ��Ĵ�����������
		 * 2. ��������ڣ����п���Ĵ�����ȡһ������Ĵ����� load�������Ĵ�����������
		 * 3. ��������ڿ��мĴ�����ȡ����β�Ĵ������Ƚ��Ĵ����ڵ�ֵд���ڴ棬��load�±�������������
		*/
		string assign_reg(shared_ptr<TableEntry> var, bool load_var = true);
		vector<string> assign_reg(shared_ptr<TableEntry> var1, shared_ptr<TableEntry> var2, bool load_var = true);
		/**assign an available $t0-$t9 reg for const, immediate num; return reg name*/
		//string assign_buf_reg(string value);
		///* ����null��ʾ��ֵ�ǻ���ֵ�����豣�档����ֱ�ӱ��ٷ��� */
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


