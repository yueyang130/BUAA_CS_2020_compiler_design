#ifndef MIPS_FUNCTION_GENERATOR_
#define MIPS_FUNCTION_GENERATOR_

#include"BasicBlock.h"
#include<map>

using namespace std;

/*
函数调用栈:
*/

class MipsFunctionGenerator {
private:
	// input and output
	Function& func_;
	map<shared_ptr<TableEntry>, int>& global_var_offset_map_;
	unsigned int* sp;
	vector<string> mips_code_;

	// 生成目标代码
	/*局部变量相对于sp的偏移*/
	map<shared_ptr<TableEntry>, int>& loacal_var_offset_map_;
	void generateMipsCode();

public:
	MipsFunctionGenerator(Function& func, map<shared_ptr<TableEntry>, int>& gb_var_map);
	vector<string>& getMipsCode() { return mips_code_; }

};


#endif // !MIPS+FUNCTION_GENERATOR_


