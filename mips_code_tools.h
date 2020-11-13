#ifndef MIPS_CODE_TOOLS
#define MIPS_CODE_TOOLS

#include<string>
#include"Register.h"
#include"SymbolTable.h"
#include"Quaternion.h"
#include<map>

/*dump global var and strcon*/
/*load global samll var, 需要手动管理offset和align*/
vector<string> load_global_small_var(Quaternion& quater, map<shared_ptr<VarEntry>, int>& var_offset_map,
	unsigned int* p_addr, const unsigned int base_addr);
/*load global big var, MIPS自动管理word align*/
string load_global_big_var(Quaternion& quater);
/*load str*/
string load_strcon(ImmediateEntry& inum);

// alloc stack space for local var
string decalre_local_var(Quaternion& quater, map<shared_ptr<VarEntry>, int>& var_offset_map, int* framesize);

/*label and jump*/
string set_label(Quaternion& quater);
// beq, bne, blt, ble, bgt, bge
void conditional_jump(Quaternion& quater, map<shared_ptr<VarEntry>, int>& var_offset_map, vector<string>& mips_list);
void j(Quaternion& quater, vector<string>& mips_list);

/*store and load and assign*/
// 如果var在寄存器中，返回寄存器编号。否则分配一个寄存器给var，返回编号
string find_reg(shared_ptr<TableEntry> var, map<shared_ptr<VarEntry>, int>& var_offset_map, vector<string>& mips_list);

// sw, sb
string store(shared_ptr<TableEntry> var, map<shared_ptr<VarEntry>, int>& var_offset_map);
string store(string reg, int offset);
// li, move
string assign(Quaternion& quater);


/*array*/
vector<string> loadArrayElem(Quaternion& quater);
vector<string> storeArrayElem(Quaternion& quater);

/*ALU and assign*/
string alu_i(shared_ptr<TableEntry> result, shared_ptr<TableEntry> left, shared_ptr<TableEntry>, QuaternionType type);
string alu_reg(shared_ptr<TableEntry> result, shared_ptr<TableEntry> left, shared_ptr<TableEntry>, QuaternionType type);

/*read and write*/
string read(shared_ptr<TableEntry> var, map<shared_ptr<VarEntry>, int>& var_offset_map, vector<string>& mips_list);
string write(Quaternion& quater, map<shared_ptr<VarEntry>, int>& var_offset_map, vector<string>& mips_list);

#endif // !MIPS_CODE_TOOLS

