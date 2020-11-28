#ifndef MIPS_CODE_TOOLS
#define MIPS_CODE_TOOLS

#include<string>
#include"SymbolTable.h"
#include"Quaternion.h"
#include<map>

class SimpleMipsFunctionGenerator;

/*dump global var and strcon*/
/*load global samll var, 需要手动管理offset和align*/
vector<string> load_global_small_var(Quaternion& quater, map<VarEntry*, int>& var_offset_map,
	unsigned int* p_addr, const unsigned int base_addr);
/*load global big var, MIPS自动管理地址*/
string load_global_big_var(Quaternion& quater);
/*load str, MIPS自动管理地址*/
string load_strcon(ImmediateEntry& inum);


/*memory load and store*/
// lw reg1, offset(reg2) | lb
void mips_load_mem(string reg1, string reg2, int offset, ValueType type, vector<string>& mips_list);
// lw reg1, label(reg2) | lb
void mips_load_mem(string reg1, string reg2, string label, ValueType type, vector<string>& mips_list);
// la reg1, inum || la reg1, label
void mips_load_num(string reg1, string source, vector<string>& mips_list);
// move reg1, reg2
void mips_load_reg(string reg1, string reg2, vector<string>& mips_list);
// sw reg1, offset(reg2) | sb
void mips_store(string reg1, string reg2, int offset, ValueType type, vector<string>& mips_list);
// sw reg1, label(reg2) | sb
void mips_store(string reg1, string reg2, string label, ValueType type, vector<string>& mips_list);

/*label and jump*/
void set_label(Quaternion& quater, vector<string>& mips_list);
// beq, bne, blt, ble, bgt, bge
void conditional_jump(string reg1, string reg2, string label, QuaternionType type, vector<string>& mips_list);
void mips_j(Quaternion& quater, vector<string>& mips_list);
//void mips_jr(vector<string>& mips_list);

/*function*/
void mips_jal(string func_name, vector<string>& mips_list);
void mips_jr(bool ismain, vector<string>& mips_list);

/*array*/
/*
reg0: 存放最终相对于数组头偏移量结果的寄存器
reg1: 用于中间运算的寄存器
entry: 数组变量的symbol Table entry
reg_idxs: 存有数组下标的寄存器名的数组
*/
void off_in_array(string reg0, string reg1, shared_ptr<TableEntry> entry, vector<string> reg_idxs, vector<string>& mips_list);


/*ALU: result and right must be register; right can be a register or a immediate num*/
void mips_alu(string result, string left, string right, QuaternionType type, vector<string>& mips_list);


/*read and write*/
// 函数调用后，读入的值存在$v0
void read(ValueType type, vector<string>& mips_list);
void write_str(string str_name, vector<string>& mips_list);
// 在进入函数前应该保证要打印的值已经存在$a0中
void write_expr(ValueType type, vector<string>& mips_list);
// 输出换行
void write_lf(vector<string>& mips_list);




#endif // !MIPS_CODE_TOOLS

