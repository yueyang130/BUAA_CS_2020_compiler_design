#include "RegisterPool.h"
#include "OptMipsFunctionGenerator.h"
#include"tools.h"

using OptMips::RegisterPool;

RegisterPool::RegisterPool() {
    for (int i = 0; i < T_NUM; i++) {
        t_regs[i] = nullptr;
    }
}

void RegisterPool::write_back(unordered_set<shared_ptr<TableEntry>> active_set) {
    for (int i = 0; i < T_NUM; i++) {
        if (t_regs[i] == nullptr) continue;
        if (t_regs[i]->entry_type() == EntryType::IMMEDIATE) continue;
        if (active_set.find(t_regs[i]) != active_set.end()) {
            func_generator_->write_back(t_regs[i], treg_name(i));
        }
    }
}

void RegisterPool::save_tregs(unordered_set<shared_ptr<TableEntry>> active_set, vector<string>& save_list) {
    save_list.clear();
    int offset = 0;
    for (int i = 0; i < T_NUM; i++) {
        if (t_regs[i] == nullptr) continue;
        if (t_regs[i]->entry_type() == EntryType::IMMEDIATE) continue;
        if (active_set.find(t_regs[i]) != active_set.end()) {
            offset += 4;
            mips_store(treg_name(i), "$sp", -offset, ValueType::INTV, func_generator_->mips_list_);
            save_list.push_back(treg_name(i));
        }
    }
}

void RegisterPool::restore_tregs(vector<string>& save_list) {
    int offset = 0;
    for (auto reg : save_list) {
        offset += 4;
        mips_load_mem(reg, "$sp", -offset, ValueType::INTV, func_generator_->mips_list_);
    }
    save_list.clear();
}

void RegisterPool::clearTempRegs() {
   
    for (int i = 0; i < T_NUM; i++) {
        t_regs[i] = nullptr;
    }
    
}

string RegisterPool::assign_temp_reg(shared_ptr<TableEntry> var, bool load_var) {

    if (const_or_immed(var.get())) {
        mips_load_num(buf_reg1, var->getValue(), func_generator_->mips_list_);
        return buf_reg1;
    }

    int index;
    if (find(var, &index)) {
        lru_queue_.insert(lru_queue_.begin(), index);
        return treg_name(index);
    }

    if (hasEmptyTReg(&index)) {
        t_regs[index] = var;
        lru_queue_.insert(lru_queue_.begin(), index);
        if (load_var) {
            func_generator_->load_var(var, treg_name(index));
        }
        return treg_name(index);
    } 
    index = lru_queue_.back();
    lru_queue_.pop_back();
    // 写回
    func_generator_->write_back(t_regs[index], treg_name(index));
    t_regs[index] = var;
    lru_queue_.insert(lru_queue_.begin(), index);
    if (load_var) {
        func_generator_->load_var(var, treg_name(index));
    }
    return treg_name(index);
}

vector<string> RegisterPool::assign_temp_reg(shared_ptr<TableEntry> var1, shared_ptr<TableEntry> var2, bool load_var) {
    if (const_or_immed(var1.get()) && const_or_immed(var2.get())) {
        mips_load_num(buf_reg1, var1->getValue(), func_generator_->mips_list_);
        mips_load_num(buf_reg2, var2->getValue(), func_generator_->mips_list_);
        return { buf_reg1, buf_reg2 };
    }
    return { assign_temp_reg(var1), assign_temp_reg(var2) };
} 

bool RegisterPool::find(shared_ptr<TableEntry> var, int* p_index) {

    for (int i = 0; i < T_NUM; i++) {
        if (t_regs[i].get() == var.get()) {
            *p_index = i;
            return true;
        }
    }

    return false;
}

bool RegisterPool::find(string value, int* p_index) {

    for (int i = 0; i < T_NUM; i++) {
        if (!t_regs[i]) continue;
        if (t_regs[i]->entry_type() == EntryType::IMMEDIATE && t_regs[i]->getValue() == value) {
            *p_index = i;
            return true;
        }
    }

    return false;
}

//bool RegisterPool::hasEmptySReg(int* p_index) {
//    for (int i = 0; i < S_NUM; i++) {
//        if (s_regs[i].get() == nullptr) {
//            *p_index = i;
//            return true;
//        }
//    }
//    return false;
//}

bool RegisterPool::hasEmptyTReg(int* p_index) {
    for (int i = 0; i < T_NUM; i++) {
        if (t_regs[i].get() == nullptr) {
            *p_index = i;
            return true;
        }
    }
    return false;
}

//string RegisterPool::assign_global_reg(shared_ptr<TableEntry> var) {
//    int index;
//    if (hasEmptyGbReg(&index)) {
//        return "$s" + to_string(index);
//    }
//    // 如果没有空闲寄存器，目前先随便换出一个
//    write_back(S_REG, S_NUM - 1);
//}
