#ifndef SIMPLE_MIPS_FUNCTION_GENERATOR_
#define SIMPLE_MIPS_FUNCTION_GENERATOR_

#include"BasicBlock.h"
#include"simple_mips_code_tools.h"
#include<map>

using namespace std;

/*
simple dump:
Ϊÿһ���ֲ���������ʱ����������һ���ڴ�ռ��ַ��
�Ĵ���ֻʹ��$a0,$a1,$a2. 
ÿ��Ҫʹ�ñ���ʱload,������������д���ڴ�ռ��ַ
*/

const string reg0 = "$a0";
const string reg1 = "$a1";
const string reg2 = "$a2";

class SimpleMipsFunctionGenerator {
private:
	// input and output
	Function& func_;
	map<shared_ptr<VarEntry>, int>& global_var_offset_map_;
	unsigned int* sp;
	vector<string>& mips_list_;

	/*ƫ����*/
	int offset = 0;
	/*�ֲ���������ʱ���������sp��ƫ��*/
	map<shared_ptr<TableEntry>, int> func_var_offset_map_;

	/* ����ֲ�����������Ϊ�ֲ�������ջ�з����ڴ�ռ��ַ */
	void map_local_var();
	/* �������ڵ���ʱ������Ϊ��ʱ������ջ�з����ڴ�ռ��ַ */
	void map_temp_var();
	/* ����Ǿֲ���������ʱ���������ڴ��еı������ص��Ĵ���*/
	/* ���������������const, ͨ��la����*/
	void load_var(shared_ptr<TableEntry> var, string reg);
	/* ���Ĵ����е�ֵ�洢���ֲ���������ʱ������Ӧ���ڴ��ַ*/
	void store_var(shared_ptr<TableEntry> var, string reg);

public:
	SimpleMipsFunctionGenerator(Function& func, map<shared_ptr<VarEntry>, int>& gb_var_map, vector<string>& mips_list_);
	vector<string>& getMipsCode() { return mips_list_; }

};

/* ֧�ֺ��� */
// �ж��Ƿ�����ʱ����
bool isTempVar(shared_ptr<TableEntry> entry);


#endif // !SIMPLE_MIPS_FUNCTION_GENERATOR_


