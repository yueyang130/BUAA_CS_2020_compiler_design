//#include "RegisterPool.h"
//
//RegisterPool* myReg = new RegisterPool();
//
//RegisterPool::RegisterPool(vector<string>& list) :
//    mips_list(list)
//{
//}
//
//bool RegisterPool::find(shared_ptr<TableEntry> var, string* p_str) {
//    for (int i = 0; i < A_NUM; i++) {
//        if (a_regs[i].get() == var.get()) {
//            *p_str = "$a" + to_string(i);
//            return true;
//        }
//    }
//    for (int i = 0; i < T_NUM; i++) {
//        if (a_regs[i].get() == var.get()) {
//            *p_str = "$t" + to_string(i);
//            return true;
//        }
//    }
//    for (int i = 0; i < S_NUM; i++) {
//        if (a_regs[i].get() == var.get()) {
//            *p_str = "$s" + to_string(i);
//            return true;
//        }
//    }
//    for (int i = 0; i < V_NUM; i++) {
//        if (a_regs[i].get() == var.get()) {
//            *p_str = "$v" + to_string(i);
//            return true;
//        }
//    }
//    return false;
//}
//
//bool RegisterPool::hasEmptyGbReg(int* p_index) {
//    for (int i = 0; i < S_NUM; i++) {
//        if (s_regs[i].get() == nullptr) {
//            *p_index = i;
//            return true;
//        }
//    }
//    return false;
//}
//
//bool RegisterPool::hasEmptyTpReg(int* p_index) {
//    for (int i = 0; i < T_NUM; i++) {
//        if (t_regs[i].get() == nullptr) {
//            *p_index = i;
//            return true;
//        }
//    }
//    return false;
//}
//
//string RegisterPool::assign_global_reg(shared_ptr<TableEntry> var) {
//    int index;
//    if (hasEmptyGbReg(&index)) {
//        return "$s" + to_string(index);
//    }
//    // 如果没有空闲寄存器，目前先随便换出一个
//    write_back(S_REG, S_NUM - 1);
//}
//
//string RegisterPool::assign_temp_reg(shared_ptr<TableEntry> var) {
//    int index;
//    if (hasEmptyTpReg(&index)) {
//        return "$t" + to_string(index);
//    }
//    // 如果没有空闲寄存器，目前先随便换出一个
//    write_back(T_REG, T_NUM - 1);
//}
//
//string RegisterPool::assign_value_to_reg(shared_ptr<TableEntry> entry, RegType type, int index) {
//    string reg;
//    if (myReg->find(entry, &reg)) {
//        return reg;
//    }
//    reg = myReg->assign_global_reg(entry);
//    return reg;
//}
//
//void RegisterPool::assign_reg_to_var(shared_ptr<TableEntry> entry, RegType type, int index) {
//    string reg;
//    if (find(entry, &reg)) {
//        mips_list.push_back("move " + reg )
//    }
//}
//
//void RegisterPool::write_back(RegType type, int index) {
//
//}
//
