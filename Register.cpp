#include "Register.h"

Register* myReg = new Register();

Register::Register(vector<string>& list) :
    mips_list(list)
{
}

bool Register::find(shared_ptr<TableEntry> var, string* p_str) {
    for (int i = 0; i < A_NUM; i++) {
        if (a_regs[i].get() == var.get()) {
            *p_str = "$a" + to_string(i);
            return true;
        }
    }
    for (int i = 0; i < T_NUM; i++) {
        if (a_regs[i].get() == var.get()) {
            *p_str = "$t" + to_string(i);
            return true;
        }
    }
    for (int i = 0; i < S_NUM; i++) {
        if (a_regs[i].get() == var.get()) {
            *p_str = "$s" + to_string(i);
            return true;
        }
    }
    for (int i = 0; i < V_NUM; i++) {
        if (a_regs[i].get() == var.get()) {
            *p_str = "$v" + to_string(i);
            return true;
        }
    }
    return false;
}

bool Register::hasEmptyGbReg(int* p_index) {
    for (int i = 0; i < S_NUM; i++) {
        if (a_regs[i].get() == nullptr) {
            *p_index = i;
            return true;
        }
    }
    return false;
}

bool Register::hasEmptyTpReg(int* p_index) {
    for (int i = 0; i < T_NUM; i++) {
        if (t_regs[i].get() == nullptr) {
            *p_index = i;
            return true;
        }
    }
    return false;
}

string Register::assign_global_reg(shared_ptr<TableEntry> var, map<shared_ptr<TableEntry>, int>& var_offset_map) {
    return string();
}

string Register::assign_temp_reg(shared_ptr<TableEntry> var, map<shared_ptr<TableEntry>, int>& var_offset_map) {
    return string();
}

string Register::assign_value_to_reg(shared_ptr<TableEntry> entry, RegType type, int index) {
    return string();
}

void Register::assign_ret_to_var(shared_ptr<TableEntry> entry, RegType type, int index) {
}
