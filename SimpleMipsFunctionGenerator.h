#ifndef SIMPLE_MIPS_FUNCTION_GENERATOR_
#define SIMPLE_MIPS_FUNCTION_GENERATOR_

#include"BasicBlock.h"
#include"simple_mips_code_tools.h"
#include<map>

using namespace std;

/*
simple dump:
为每一个局部变量和临时变量都分配一个内存空间地址，
寄存器只使用$a0,$a1,$a2. 
每次要使用变量时load,完成运算后立刻写回内存空间地址
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

	/*偏移量*/
	int offset = 0;
	/*局部变量和临时变量相对于sp的偏移*/
	map<shared_ptr<TableEntry>, int> func_var_offset_map_;

	/* 处理局部变量声明，为局部变量在栈中分配内存空间地址 */
	void map_local_var();
	/* 处理函数内的临时变量，为临时变量在栈中分配内存空间地址 */
	void map_temp_var();
	/* 如果是局部变量和临时变量，将内存中的变量加载到寄存器*/
	/* 如果是立即数或者const, 通过la加载*/
	void load_var(shared_ptr<TableEntry> var, string reg);
	/* 将寄存器中的值存储到局部变量和临时变量对应的内存地址*/
	void store_var(shared_ptr<TableEntry> var, string reg);

public:
	SimpleMipsFunctionGenerator(Function& func, map<shared_ptr<VarEntry>, int>& gb_var_map, vector<string>& mips_list_);
	vector<string>& getMipsCode() { return mips_list_; }

};

/* 支持函数 */
// 判断是否是临时变量
bool isTempVar(shared_ptr<TableEntry> entry);


#endif // !SIMPLE_MIPS_FUNCTION_GENERATOR_


