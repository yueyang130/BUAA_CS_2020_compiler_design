#ifndef SIMPLE_MIPS_GENERATOR_H_
#define SIMPLE_MIPS_GENERATOR_H_

#include"BasicBlock.h"
#include"Quaternion.h"
#include"SimpleMipsFunctionGenerator.h"
#include<map>
#include<vector>
#include<iostream>

using namespace std;

const unsigned int GP_ADDR =   0x10008000;
const unsigned int DATA_ADDR = 0x10010000;


/*
MipsGenerator:
1.��Ϊ��Ԫʽ�Ĳ�������TableEntry,���Բ���Ҫ���Ǳ�������������
	����һ���Ѿ��ڷ��ű�����������п��ǣ�
2.�����Ĵ���
			ֱ��ʹ��Ӳ���루��ConstEntry�ķ������Ӳ���룩
3.ȫ�ֱ�����String�Ĵ���
			С����(int��char)ͨ��.data�洢��gp����,ͨ��gp+offset����
			�����(array��String)ͨ��.data�洢��.data�Σ�ͨ��addr����
4.�ֲ���������ʱ�����ĵĴ���:
			��MipsFunctionGenerator
*/

class MipsGenerator {
public:
	static MipsGenerator& getInstance(IMCode& im_code);
	/*generate mips code*/
	void generateMipsCode();
	void showMipsCode(ostream& fout);

private:
	MipsGenerator(IMCode& im_code);

	// input and output
	IMCode& im_code_;
	vector<string> MipsCode_list_;
	
	/*int,char���͵�ȫ�ֱ��������$gp��ƫ��*/
	map<VarEntry*, int> gb_small_var_offset_map_;
	/*samll global var�ķ����ַ*/
	unsigned int _pglobal = GP_ADDR;
	/*array���͵�ȫ�ֱ�����String�����ľ��Ե�ַ�������ǹ���MIPS�Զ�����*/

	
	/*store global var in data segment*/
	void dump_global_and_strcon(); 
	void dump_main();

};

/*֧�ֺ���*/

/*�����strcon���͵�ImmediateEntry,ע��dumpָ��*/
void dump_strcon(shared_ptr<TableEntry> p_entry, vector<string>& instr_list);


#endif // !SIMPLE_MIPS_GENERATOR_H_



