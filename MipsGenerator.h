#ifndef MIPS_GENERATOR_H_
#define MIPS_GENERATOR_H_

#include"BasicBlock.h"
#include"Quaternion.h"
#include<map>
#include<vector>
#include<iostream>

using namespace std;

const unsigned int GP_ADDR =   0x10008000;
const unsigned int DATA_ADDR = 0x10010000;


/*
MipsGenerator:
1.因为四元式的操作数是TableEntry,所以不需要考虑变量重名的问题
	（这一点已经在符号表建立与错误处理中考虑）
2.常量的处理：
			直接使用硬编码（由ConstEntry的方法获得硬编码）
3.全局变量和String的处理：
			小变量(int和char)通过.data存储在gp附近,通过gp+offset访问
			大变量(array和String)通过.data存储在.data段，通过addr访问
4.局部变量的处理:
			
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
	
	/*int,char类型的全局变量相对于$gp的偏移*/
	map<shared_ptr<VarEntry>, int> gb_var_offset_map_;
	/*samll global var的分配地址*/
	unsigned int _pglobal = GP_ADDR;
	/*array类型的全局变量和String常量的绝对地址无需我们管理，MIPS自动管理*/

	
	/*store global var in data segment*/
	void dump_data_segment(); 
	void dump_main();

};

/*支持函数*/

/*如果是strcon类型的ImmediateEntry,注册dump指令*/
void dump_strcon(shared_ptr<TableEntry> p_entry, vector<string>& instr_list);


#endif // !MIPS_GENERATOR_H_



