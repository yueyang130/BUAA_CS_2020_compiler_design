#ifndef OPT_MIPS_FUNCTION_GENERATOR_
#define OPT_MIPS_FUNCTION_GENERATOR_

#include"ImCoder.h"
#include"simple_mips_code_tools.h"
#include<map>

using namespace std;

/*
simple dump:
Ϊÿһ���ֲ���������ʱ����������һ���ڴ�ռ��ַ��
�Ĵ���ֻʹ��$a0,$a1,$a2. 
ÿ��Ҫʹ�ñ���ʱload,������������д���ڴ�ռ��ַ

ջ����:
	a,v,t   (���Ҫ��������ǰ�󱣳�һ�£����ɵ��÷�����,simple������迼����һ��)
	|
	func param
	|
------  fp
	|
	ra
	|
	prev fp
	|
	s0-s7  (����Ӻ���ʹ�ã�����뱣��,simple������迼����һ��)
	|
	�ֲ�����
	|
	��ʱ����
	|
------  sp
*/

namespace SimpleMips {

	const string reg0 = "$s0";
	const string reg1 = "$s1";
	const string reg2 = "$s2";

	class SimpleMipsFunctionGenerator {
	private:
		// input and output
		Function& func_;
		map<VarEntry*, int>& global_var_offset_map_;
		vector<string>& mips_list_;

		/*ƫ����*/
		int offset = 0;
		/*�ֲ���������ʱ���������fp��ƫ��*/
		map<TableEntry*, int> func_var_offset_map_;

		/* ����ֲ�����������Ϊ�ֲ�������ջ�з����ڴ�ռ��ַ */
		void map_local_var();
		/* �������ڵ���ʱ������Ϊ��ʱ������ջ�з����ڴ�ռ��ַ */
		void map_temp_var();
		/* ����Ǿֲ���������ʱ���������ڴ��еı������ص��Ĵ���*/
		/* ���������������const, ͨ��la����*/
		void load_var(shared_ptr<TableEntry> var, string reg);
		/* ���Ĵ����е�ֵ�洢���ֲ���������ʱ������Ӧ���ڴ��ַ*/
		void store_var(shared_ptr<TableEntry> var, string reg);
		/*
		���ڴ��е�����Ԫ�ؼ��ص��Ĵ���
		off_reg:  �������������ͷƫ�����ļĴ���
		target_reg: ��Ҫ�洢��ֵ��Ŀ��Ĵ���
		ע�⣺�˺������ú�offset��ֵ���ᷢ���ı�
		*/
		void load_array(shared_ptr<TableEntry> var, string off_reg, string target_reg);
		/*
		���Ĵ����е�ֵ�洢������Ԫ�ض�Ӧ���ڴ��ַ
		off_reg:  �������������ͷƫ�����ļĴ���
		source_reg: �洢��ֵ�ļĴ���
		ע�⣺�˺������ú�offset��ֵ���ᷢ���ı�
		*/
		void store_array(shared_ptr<TableEntry> var, string off_reg, string source_reg);

	public:
		SimpleMipsFunctionGenerator(Function& func, map<VarEntry*, int>& gb_var_map, vector<string>& mips_list_);
		vector<string>& getMipsCode() { return mips_list_; }

	};

	/* ֧�ֺ��� */
	// �ж��Ƿ�����ʱ����
	bool isTempVar(shared_ptr<TableEntry> entry);

}

#endif // !OPT_MIPS_FUNCTION_GENERATOR_


