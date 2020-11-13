#include"mips_code_tools.h"
#include<assert.h>
#include<sstream>

const int MAXLEN = 1000;
char buf[MAXLEN];

vector<string> load_global_small_var(Quaternion& quater, map<shared_ptr<VarEntry>, int>& var_offset_map,
	unsigned int * p_addr, const unsigned int base_addr) {

	assert(quater.quater_type_ == QuaternionType::VarDeclare);

	shared_ptr<VarEntry> p_var = dynamic_pointer_cast<VarEntry>(quater.result_);
	ValueType type = p_var->value_type();
	vector<string> rets;
	string load_instr = p_var->identifier();

	if (quater.left_) {
		ImmediateEntry& inum = *dynamic_pointer_cast<ImmediateEntry>(quater.left_);
		if (type == ValueType::INTV) {
			// 整数对齐分配
			int res = *p_addr % 4;
			if (res != 0) {
				*p_addr = *p_addr + 4 - res;
				rets.push_back(".align 2");
			}
			load_instr += ":.word " + inum.identifier();
			var_offset_map[p_var] = *p_addr - base_addr;
			*p_addr += 4;

		} else {
			// byte分配不用对齐
			load_instr += ":.byte \'" + inum.identifier() + "\'";
			var_offset_map[p_var] = *p_addr - base_addr;
			*p_addr += 1;
		}

	} else {
		if (type == ValueType::INTV) {
			// 整数对齐分配
			int res = *p_addr % 4;
			if (res != 0) {
				*p_addr = *p_addr + 4 - res;
				rets.push_back(".align 2");
			}
			load_instr += ":.space 4";
			var_offset_map[p_var] = *p_addr - base_addr;
			*p_addr += 4;

		} else {
			// byte分配不用对齐
			load_instr += ":.space 1";
			var_offset_map[p_var] = *p_addr - base_addr;
			*p_addr += 1;
		}
	}
	rets.push_back(load_instr);

	return rets;
}

string load_global_big_var(Quaternion& quater) {
	assert(quater.quater_type_ == QuaternionType::VarDeclare);
	VarEntry& var = *dynamic_pointer_cast<VarEntry>(quater.result_);
	ValueType type = var.value_type();
	stringstream ret = stringstream(var.identifier());
	//int byte_size = 0;

	// 有初始化
	if (quater.left_) {
		ImmediateEntry& inum = *dynamic_pointer_cast<ImmediateEntry>(quater.left_);
		if (type == ValueType::INTV) {
			ret << ":.word ";
			for (auto& x : inum.initializingList()) {
				ret << x << ", ";
				//byte_size += 4;
			}

		} else {
			ret << ":.byte ";
			for (auto& x : inum.initializingList()) {
				ret << "\'" << x << "\', ";
				//byte_size += 1;
			}
		}

	} else {
	// 无初始化
		ret << ":.space " << var.ByteSize();
		//byte_size += 1;
	}

	// 字节可能存在对齐问题
	/*if (byte_size % 4 == 0) {
		rets.push_back(".align 2");
	}*/
	
	return ret.str();
}

string load_strcon(ImmediateEntry& inum) {
	assert(inum.value_type() == ValueType::STRINGV);
	//vector<string> instrs;

	string ret = inum.identifier();
	ret += ":.asciiz \"" + inum.getValue() + "\"";
	//instrs.push_back(ret);
	return ret;
}

/*分配全局寄存器，如果没有，则存入Stack*/
string decalre_local_var(Quaternion& quater, map<shared_ptr<VarEntry>, int>& var_offset_map, int* framesize) {
	const shared_ptr<VarEntry> var =  dynamic_pointer_cast<VarEntry>(quater.result_);
	string instr = "";

	if (!quater.left_.get()) { // 有初始化
		const shared_ptr<ImmediateEntry> inum = dynamic_pointer_cast<ImmediateEntry>(quater.left_);
		if (myReg->hasEmptyGbReg()) {
			instr = "li " + myReg->assign_global_reg() + inum->getValue();
		} else {
			instr = store(myReg->assign_global_reg(), *framesize);
		}
	} 
	
	var_offset_map[var] = *framesize;

	if (var->value_type() == ValueType::INTV) {
		*framesize += 4;
	} else if (var->value_type() == ValueType::CHARV) {
		*framesize += 1;
	}
}

string set_label(Quaternion& quater) {
	assert(quater.quater_type_ == QuaternionType::Label);
	return quater.result_->identifier() + " :";
}

string conditional_jump(Quaternion& quater, map<shared_ptr<VarEntry>, int>& var_offset_map, vector<string>& mips_list) {

	string instr;
	switch (quater.quater_type_) {
	case QuaternionType::BEQ:
		instr = "beq";
	case QuaternionType::BNE:
		instr = "bne";
	case QuaternionType::BLT:
		instr = "blt";
	case QuaternionType::BLE:
		instr = "ble";
	case QuaternionType::BGT:
		instr = "bgt";
	case QuaternionType::BGE:
		instr = "bge";
	default:
		instr = "error";
	}
	string reg1 = load(quater.left_, var_offset_map, mips_list);
	string reg2 = load(quater.right_, var_offset_map, mips_list);
	instr += " " + reg1 + ", " + reg2 + ", " + quater.result_->identifier();
	mips_list.push_back(instr);
}

string j(Quaternion& quater, vector<string>& mips_list) {
	mips_list.push_back("j " + quater.result_->identifier());
}

string load(shared_ptr<TableEntry> var, map<shared_ptr<VarEntry>, int>& var_offset_map, vector<string>& mips_list) {
	string reg;
	if (myReg->find(var, &reg)) {
		return reg;
	}
	reg = myReg->assign_global_reg(var, var_offset_map, mips_list);
	return reg;
}

string load(string reg, int offset) {
	return string();
}

string store(shared_ptr<TableEntry> var, map<shared_ptr<VarEntry>, int>& var_offset_map) {
	string reg;
	if (myReg->find(var, &reg)) {
		return reg;
	}
	// store in memory
	return reg;
}

string store(string reg, int offset) {
	return string();
}

string assign(Quaternion& quater) {

}

vector<string> loadArrayElem(Quaternion& quater) {
	return vector<string>();
}

vector<string> storeArrayElem(Quaternion& quater) {
	return vector<string>();
}

string alu_i(shared_ptr<TableEntry> result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right QuaternionType type) {
	string reg1 = myReg->assign_temp_reg();
	string reg2 = myReg->assign_temp_reg();
	string instr;
	switch (type) {
	case QuaternionType::AddOp:
		instr = "Add " + reg1 + ", " + reg2 + ", " + right->identifier();
		break;
	case QuaternionType::SubOp:
		instr = "Sub " + reg1 + ", " + reg2 + ", " + right->identifier();
		break;
	case QuaternionType::MulOp:
		instr = "Mul " + reg1 + ", " + reg2 + ", " + right->identifier();
		break;
	case QuaternionType::DivOp:
		instr = "Div " + reg1 + ", " + reg2 + ", " + right->identifier();
		break;
	default:
		break;
	}
}

string alu_reg(shared_ptr<TableEntry> result, shared_ptr<TableEntry> left, shared_ptr<TableEntry>, QuaternionType type) {
	return string();
}

string read(shared_ptr<TableEntry> var, map<shared_ptr<VarEntry>, int>& var_offset_map, vector<string>& mips_list) {
	if (var->value_type() == ValueType::INTV) {
		mips_list.push_back("li $v0, 5");
	} else {
		mips_list.push_back("li $v0, 12");
	}
	mips_list.push_back("syscall");
	myReg->assign_ret_to_var(var, RegType::A_REG, 0);
}

string write(Quaternion& quater, map<shared_ptr<VarEntry>, int>& var_offset_map, vector<string>& mips_list) {
	if (quater.left_.get()) {
		sprintf(buf, "la $a0 %s", quater.left_->identifier().c_str());
		mips_list.push_back(buf);
		mips_list.push_back("li $v0, 4");
		mips_list.push_back("syscall");
	}
	if (quater.right_.get()) {
		myReg->assign_value_to_reg(quater.right_, RegType::A_REG, 0);
		if (quater.right_->value_type() == ValueType::INTV) {
			mips_list.push_back(buf);
			mips_list.push_back("li $v0, 1");
			mips_list.push_back("syscall");
		} else {
			mips_list.push_back(buf);
			mips_list.push_back("li $v0, 11");
			mips_list.push_back("syscall");
		}
	}

	// 输出换行
	mips_list.push_back("la $v0, 11");
	mips_list.push_back("la $a0, \'\n\'");
	mips_list.push_back("syscall");
}
