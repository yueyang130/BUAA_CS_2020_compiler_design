//#ifndef OPT_MIPS_FUNCTION_GENERATOR_
//#define OPT_MIPS_FUNCTION_GENERATOR_
//
//#include"BasicBlock.h"
//#include<map>
//
//using namespace std;
//
///*
//optimized dump:
//为每一个局部变量和临时变量都分配一个内存空间地址，
//并将部分变量映射到全局寄存器和局部寄存器
//1.使用变量时，如果变量在寄存器中则直接使用寄存器
//2.只有寄存器中没有变量时，才从内存中load变量。
//*/
//
//class MipsFunctionGenerator {
//private:
//	// input and output
//	Function& func_;
//	map<shared_ptr<TableEntry>, int>& global_var_offset_map_;
//	unsigned int* sp;
//	vector<string> mips_code_;
//
//	/*偏移量*/
//	int offset = 0;
//	/*局部变量和临时变量相对于sp的偏移*/
//	map<shared_ptr<TableEntry>, int> func_var_offset_map_;

//	/* 处理局部变量声明，为局部变量在栈中分配内存空间地址
//	   如果有全局寄存器空闲，则将变量值赋予空闲全局寄存器，
//	   否则值写到内存空间
//	   */

//	void map_local_var();
//	/* 处理函数内的临时变量，为临时变量在栈中分配内存空间地址
//	   如果有临时寄存器空闲，则将变量值赋予空临时全局寄存器，
//	   否则值写到内存空间
//	   */
//	void map_temp_var();
//
//
//public:
//	MipsFunctionGenerator(Function& func, map<shared_ptr<TableEntry>, int>& gb_var_map);
//	vector<string>& getMipsCode() { return mips_code_; }
//
//};
//
//
//#endif // !OPT_MIPS_FUNCTION_GENERATOR_
//
//
