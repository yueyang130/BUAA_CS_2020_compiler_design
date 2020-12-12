#ifndef OPT_MIPS_FUNCTION_GENERATOR_
#define OPT_MIPS_FUNCTION_GENERATOR_

#include"ImCoder.h"
#include"opt_mips_code_tools.h"
#include<map>
#include"RegisterPool.h"

using namespace std;

namespace OptMips {

	/*
	simple dump:
	为每一个局部变量和临时变量都分配一个内存空间地址，
	寄存器只使用$a0,$a1,$a2.
	每次要使用变量时load,完成运算后立刻写回内存空间地址

	栈布局:
		a,v,t   (如果要求函数调用前后保持一致，则由调用方保护,simple情况无需考虑这一点)
		|
		func param
		|
	------  fp
		|
		ra
		|
		prev fp
		|
		s0-s7  (如果子函数使用，则必须保护,simple情况无需考虑这一点)
		|
		局部变量
		|
		临时变量
		|
	------  sp


	optimized dump:
	为每一个局部变量和临时变量都分配一个内存空间地址，
	并将部分变量映射到全局寄存器和局部寄存器
	1.使用变量时，如果变量在寄存器中则直接使用寄存器
	2.只有寄存器中没有变量时，才从内存中load变量。

	*/



	class OptMipsFunctionGenerator {
		friend class RegisterPool;
	private:
		// 寄存器池
		RegisterPool& reg_pool_;
		// 
		// input and output
		Function& func_;
		map<VarEntry*, int>& global_var_offset_map_;
		vector<string>& mips_list_;

		/*偏移量*/
		int offset = 0;
		/*局部变量和临时变量相对于fp的偏移*/
		map<TableEntry*, int> func_var_offset_map_;

		/* 处理局部变量声明，为局部变量在栈中分配内存空间地址 */
		void map_local_var();
		/* 处理函数内的临时变量，为临时变量在栈中分配内存空间地址 */
		void map_temp_var();
		/* 如果是局部变量和临时变量，将内存中的变量加载到寄存器*/
		/* 如果是立即数或者const, 通过la加载*/
		void load_var(shared_ptr<TableEntry> var, string reg);
		/* 将寄存器中的值存储到局部变量和临时变量对应的内存地址*/
		void write_back(shared_ptr<TableEntry> var, string reg);
		/*
		将内存中的数组元素加载到寄存器
		off_reg:  存有相对于数组头偏移量的寄存器
		target_reg: 将要存储左值的目标寄存器
		注意：此函数调用后，offset的值将会发生改变
		*/
		void load_array(shared_ptr<TableEntry> var, string off_reg, string target_reg);
		/*
		将寄存器中的值存储到数组元素对应的内存地址
		off_reg:  存有相对于数组头偏移量的寄存器
		source_reg: 存储右值的寄存器
		注意：此函数调用后，offset的值将会发生改变
		*/
		void store_array(shared_ptr<TableEntry> var, string off_reg, string source_reg);

	public:
		OptMipsFunctionGenerator(Function& func, map<VarEntry*, int>& gb_var_map, vector<string>& mips_list_,
			RegisterPool& reg_pool, IMCode& im_code);
		vector<string>& getMipsCode() { return mips_list_; }

	};

	/* 支持函数 */
	// 判断是否是临时变量
	bool isTempVar(shared_ptr<TableEntry> entry);

}

#endif // !OPT_MIPS_FUNCTION_GENERATOR_


