#include "RegisterPool.h"
#include "OptMipsFunctionGenerator.h"
#include"tools.h"
#include<algorithm>

using OptMips::RegisterPool;
using std::find;

RegisterPool::RegisterPool() {
    for (int i = 0; i < T_NUM; i++) {
        t_regs[i] = nullptr;
    }
    for (int i = 0; i < S_NUM; i++) {
        s_regs[i] = nullptr;
    }
}

void OptMips::RegisterPool::count_var_ref(unordered_map<shared_ptr<TableEntry>, int> ref_map) {
    vector<pair<shared_ptr<TableEntry>, int>> vtMap;
    for (auto it = ref_map.begin(); it != ref_map.end(); it++)
        vtMap.push_back(make_pair(it->first, it->second));

    sort(vtMap.begin(), vtMap.end(),
        [](const pair<shared_ptr<TableEntry>, int>& x, const pair<shared_ptr<TableEntry>, int>& y) -> int {
            return x.second > y.second;
        });

    for (auto it = vtMap.begin(); it != vtMap.end(); it++)
        cout << it->first->identifier() << ':' << it->second << '\n';

    int up_bound = S_NUM < vtMap.size() ? S_NUM : vtMap.size();
    for (int i = 0; i < up_bound; i++) {
        s_regs[i] = vtMap[i].first;
    }


}

void OptMips::RegisterPool::deal_var_decalre(shared_ptr<TableEntry> var, string value, int offset) {
    string reg = assign_reg(var, false);
    if (reg == buf_reg1 || reg == buf_reg2) {
        // 加载到内存中
        mips_load_num(reg, value, func_generator_->mips_list_);
        mips_store(reg, "$sp", -offset, var->value_type(), func_generator_->mips_list_);
    } else {
        // 加载到s寄存器中
        mips_load_num(reg, value, func_generator_->mips_list_);
    }
}

void OptMips::RegisterPool::load_gb_var() {
    // load global
    for (int i = 0; i < S_NUM; i++) {
        if (s_regs[i]) {
            auto gb_var = dynamic_pointer_cast<VarEntry>(s_regs[i]);
            if (gb_var->isGlobal()) {
                func_generator_->load_var(gb_var, sreg_name(i));
            }
        }
    }
}

void OptMips::RegisterPool::clear_sreg() {
    for (int i = 0; i < S_NUM; i++) {
        if (s_regs[i]) {
            auto gb_var = dynamic_pointer_cast<VarEntry>(s_regs[i]);
            if (gb_var->isGlobal()) {
                func_generator_->write_back(gb_var, sreg_name(i));
            }
        }
        s_regs[i] = nullptr;
    }
    
}

void RegisterPool::treg_write_back(unordered_set<shared_ptr<TableEntry>> active_set) {
    for (int i = 0; i < T_NUM; i++) {
        if (t_regs[i] == nullptr) continue;
        if (t_regs[i]->entry_type() == EntryType::IMMEDIATE) continue;
        if (active_set.find(t_regs[i]) != active_set.end()) {
            func_generator_->write_back(t_regs[i], treg_name(i));
        }
    }

     //在sreg中的跨越基本块的局部变量无需写回
}

void RegisterPool::clearTempRegs() {

    for (int i = 0; i < T_NUM; i++) {
        t_regs[i] = nullptr;
    }
    lru_queue_.clear();

}

void RegisterPool::save_tregs(vector<string>& save_list) {
    save_list.clear();
    int offset = 0;
    for (int i = 0; i < T_NUM; i++) {
        if (t_regs[i] == nullptr) continue;
        if (t_regs[i]->entry_type() == EntryType::IMMEDIATE) continue;
        offset += 4;
        mips_store(treg_name(i), "$sp", -offset, ValueType::INTV, func_generator_->mips_list_);
        save_list.push_back(treg_name(i));
    }

    for (int i = 0; i < S_NUM; i++) {
        if (s_regs[i] == nullptr) continue;
        offset += 4;
        mips_store(sreg_name(i), "$sp", -offset, ValueType::INTV, func_generator_->mips_list_);
        save_list.push_back(sreg_name(i));
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




string RegisterPool::assign_reg(shared_ptr<TableEntry> var, bool load_var) {

    // 为局部变量和全局变量分配寄存器
    if (var->entry_type() == EntryType::VAR) {
        return assign_s_reg(var, load_var);
    }

    // 为立即数分配buf_reg
    if (const_or_immed(var.get())) {
        mips_load_num(buf_reg1, var->getValue(), func_generator_->mips_list_);
        return buf_reg1;
    }

    // TempVar分配treg
    if (var->entry_type() == EntryType::TEMP) {
        return assign_temp_reg(var, load_var);
    }

    cout << "can't assign a register for var " << var->identifier() << endl;
    exit(EXIT_FAILURE);
}

vector<string> RegisterPool::assign_reg(shared_ptr<TableEntry> var1, shared_ptr<TableEntry> var2, bool load_var) {
    if (const_or_immed(var1.get()) && const_or_immed(var2.get())) {
        mips_load_num(buf_reg1, var1->getValue(), func_generator_->mips_list_);
        mips_load_num(buf_reg2, var2->getValue(), func_generator_->mips_list_);
        return { buf_reg1, buf_reg2 };
    }
    return { assign_reg(var1), assign_reg(var2) };
} 


string OptMips::RegisterPool::assign_temp_reg(shared_ptr<TableEntry> var, bool load_var) {
    int index;
    if (find_var_in_treg(var, &index)) {
        auto it = std::find(lru_queue_.begin(), lru_queue_.end(), index);
        lru_queue_.erase(it);
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

string OptMips::RegisterPool::assign_s_reg(shared_ptr<TableEntry> var, bool load_var) {
    // 为引用较多的local and global var分配areg, 在声明时就直接加载到寄存器中
    int index;
    if (find_var_in_sreg(var, &index)) {
        return sreg_name(index);
    }
    // 为引用较少的local and global var分配treg, 如果loadvar为true，需要先load，使用后写回
    return assign_temp_reg(var, load_var);

}


bool RegisterPool::find_var_in_treg(shared_ptr<TableEntry> var, int* p_index) {

    for (int i = 0; i < T_NUM; i++) {
        if (t_regs[i].get() == var.get()) {
            *p_index = i;
            return true;
        }
    }

    return false;
}

bool RegisterPool::find_var_in_sreg(shared_ptr<TableEntry> var, int* p_index) {

    for (int i = 0; i < S_NUM; i++) {
        if (s_regs[i].get() == var.get()) {
            *p_index = i;
            return true;
        }
    }

    return false;
}

//bool RegisterPool::find(string value, int* p_index) {
//
//    for (int i = 0; i < T_NUM; i++) {
//        if (!t_regs[i]) continue;
//        if (t_regs[i]->entry_type() == EntryType::IMMEDIATE && t_regs[i]->getValue() == value) {
//            *p_index = i;
//            return true;
//        }
//    }
//
//    return false;
//}



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

