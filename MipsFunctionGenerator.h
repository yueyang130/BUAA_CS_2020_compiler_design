#ifndef MIPS_FUNCTION_GENERATOR_
#define MIPS_FUNCTION_GENERATOR_

#include"BasicBlock.h"
#include<map>

using namespace std;

/*
��������ջ:
*/

class MipsFunctionGenerator {
private:
	// input and output
	Function& func_;
	map<shared_ptr<TableEntry>, int>& global_var_offset_map_;
	unsigned int* sp;
	vector<string> mips_code_;

	// ����Ŀ�����
	/*�ֲ����������sp��ƫ��*/
	map<shared_ptr<TableEntry>, int>& loacal_var_offset_map_;
	void generateMipsCode();

public:
	MipsFunctionGenerator(Function& func, map<shared_ptr<TableEntry>, int>& gb_var_map);
	vector<string>& getMipsCode() { return mips_code_; }

};


#endif // !MIPS+FUNCTION_GENERATOR_


