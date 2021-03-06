#include"simple_mips_code_tools.h"
#include<assert.h>
#include<sstream>

namespace SimpleMips {

	static const int MAXLEN = 1000;
	static char buf[MAXLEN];

	vector<string> load_global_small_var(Quaternion& quater, map<VarEntry*, int>& var_offset_map,
		unsigned int* p_addr, const unsigned int base_addr) {

		assert(quater.quater_type_ == QuaternionType::VarDeclare);

		shared_ptr<VarEntry> p_var = dynamic_pointer_cast<VarEntry>(quater.result_);
		ValueType type = p_var->value_type();
		vector<string> rets;
		string load_instr = p_var->identifier();

		if (quater.opA_) {
			ImmediateEntry& inum = *dynamic_pointer_cast<ImmediateEntry>(quater.opA_);
			if (type == ValueType::INTV) {
				// 整数对齐分配
				int res = *p_addr % 4;
				if (res != 0) {
					*p_addr = *p_addr + 4 - res;
					rets.push_back(".align 2");
				}
				load_instr += ":.word " + inum.identifier();
				var_offset_map[p_var.get()] = *p_addr - base_addr;
				*p_addr += 4;

			} else {
				// byte分配不用对齐
				load_instr += ":.byte " + inum.identifier();
				var_offset_map[p_var.get()] = *p_addr - base_addr;
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
				var_offset_map[p_var.get()] = *p_addr - base_addr;
				*p_addr += 4;

			} else {
				// byte分配不用对齐
				load_instr += ":.space 1";
				var_offset_map[p_var.get()] = *p_addr - base_addr;
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
		stringstream ret = stringstream();
		ret << var.identifier();
		//int byte_size = 0;

		// 有初始化
		if (quater.opA_) {
			ImmediateEntry& inum = *dynamic_pointer_cast<ImmediateEntry>(quater.opA_);
			if (type == ValueType::INTV) {
				ret << ":.word ";
				for (auto& x : inum.initializingList()) {
					ret << x << ", ";
					//byte_size += 4;
				}

			} else {
				ret << ":.byte ";
				for (auto& x : inum.initializingList()) {
					ret << x << ", ";
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
		string content = inum.getValue();
		// 不转义
		for (int i = 0; i < content.length() - 1; i++) {
			//if (content[i] == '\\' && (content[i + 1] == 'n'|| content[i + 1] == 't' || content[i + 1] == '0')) {
			if (content[i] == '\\') {
				content.insert(content.begin() + i, '\\');
				i++;
			}
		}

		ret += ":.asciiz \"" + content + "\"";
		//instrs.push_back(ret);
		return ret;
	}

	void mips_load_mem(string reg1, string reg2, int offset, ValueType type, vector<string>& mips_list) {
		if (type == ValueType::INTV) {
			sprintf(buf, "lw %s, %d(%s)", reg1.c_str(), offset, reg2.c_str());
		} else {
			sprintf(buf, "lb %s, %d(%s)", reg1.c_str(), offset, reg2.c_str());
		}
		mips_list.push_back(buf);
	}

	void mips_load_mem(string reg1, string reg2, string label, ValueType type, vector<string>& mips_list) {
		if (type == ValueType::INTV) {
			sprintf(buf, "lw %s, %s(%s)", reg1.c_str(), label.c_str(), reg2.c_str());
		} else {
			sprintf(buf, "lb %s, %s(%s)", reg1.c_str(), label.c_str(), reg2.c_str());
		}
		mips_list.push_back(buf);
	}

	void mips_load_num(string reg1, string source, vector<string>& mips_list) {
		sprintf(buf, "la %s, %s", reg1.c_str(), source.c_str());
		mips_list.push_back(buf);
	}

	void mips_load_reg(string reg1, string reg2, vector<string>& mips_list) {
		sprintf(buf, "move %s, %s", reg1.c_str(), reg2.c_str());
		mips_list.push_back(buf);
	}

	void mips_store(string reg1, string reg2, int offset, ValueType type, vector<string>& mips_list) {
		if (type == ValueType::INTV) {
			sprintf(buf, "sw %s, %d(%s)", reg1.c_str(), offset, reg2.c_str());
		} else {
			sprintf(buf, "sb %s, %d(%s)", reg1.c_str(), offset, reg2.c_str());
		}
		mips_list.push_back(buf);
	}

	void mips_store(string reg1, string reg2, string label, ValueType type, vector<string>& mips_list) {
		if (type == ValueType::INTV) {
			sprintf(buf, "sw %s, %s(%s)", reg1.c_str(), label.c_str(), reg2.c_str());
		} else {
			sprintf(buf, "sb %s, %s(%s)", reg1.c_str(), label.c_str(), reg2.c_str());
		}
		mips_list.push_back(buf);
	}

	void set_label(Quaternion& quater, vector<string>& mips_list) {
		assert(quater.quater_type_ == QuaternionType::Label ||
			quater.quater_type_ == QuaternionType::FuncDeclareHead);
		mips_list.push_back(quater.result_->identifier() + " :");
	}

	void conditional_jump(string reg1, string reg2, string label, QuaternionType type, vector<string>& mips_list) {

		string instr;
		switch (type) {
		case QuaternionType::BEQ:
			instr = "beq"; break;
		case QuaternionType::BNE:
			instr = "bne"; break;
		case QuaternionType::BLT:
			instr = "blt"; break;
		case QuaternionType::BLE:
			instr = "ble"; break;
		case QuaternionType::BGT:
			instr = "bgt"; break;
		case QuaternionType::BGE:
			instr = "bge"; break;
		default:
			instr = "error";
		}

		instr += " " + reg1 + ", " + reg2 + ", " + label;
		mips_list.push_back(instr);
	}

	void mips_j(Quaternion& quater, vector<string>& mips_list) {
		mips_list.push_back("j " + quater.result_->identifier());
	}

	void mips_jal(string func_name, vector<string>& mips_list) {
		mips_list.push_back("jal " + func_name);
	}

	void mips_jr(bool ismain, vector<string>& mips_list) {

		mips_list.push_back("jr $ra");
	}

	void off_in_array(string reg0, string reg1, shared_ptr<TableEntry> entry, vector<string> reg_idxs, vector<string>& mips_list) {
		auto arr = dynamic_pointer_cast<VarEntry>(entry);
		if (arr->ndim() == 2) {
			mips_alu(reg0, reg_idxs[0], to_string(arr->getDimByte(0)), QuaternionType::MulOp, mips_list);
			mips_alu(reg1, reg_idxs[1], to_string(arr->getDimByte(1)), QuaternionType::MulOp, mips_list);
			mips_alu(reg0, reg0, reg1, QuaternionType::AddOp, mips_list);
		} else {
			mips_alu(reg0, reg_idxs[0], to_string(arr->getDimByte(0)), QuaternionType::MulOp, mips_list);
		}
	}


	void mips_alu(string result, string left, string right, QuaternionType quater_type, vector<string>& mips_list) {

		string instr;
		switch (quater_type) {
		case QuaternionType::AddOp:
			instr = "addu " + result + ", " + left + ", " + right;
			break;
		case QuaternionType::SubOp:
		case QuaternionType::Neg:
			instr = "subu " + result + ", " + left + ", " + right;
			break;
		case QuaternionType::MulOp:
			instr = "mul " + result + ", " + left + ", " + right;
			break;
		case QuaternionType::DivOp:
			instr = "div " + result + ", " + left + ", " + right;
			break;
		default:
			break;
		}
		mips_list.push_back(instr);
	}



	void read(ValueType type, vector<string>& mips_list) {
		if (type == ValueType::INTV) {
			mips_list.push_back("li $v0, 5");
			mips_list.push_back("syscall");
		} else {
			mips_list.push_back("li $v0, 12");
			mips_list.push_back("syscall");
		}
	}

	void write_str(string str_name, vector<string>& mips_list) {
		sprintf(buf, "la $a0 %s", str_name.c_str());
		mips_list.push_back(buf);
		mips_list.push_back("li $v0, 4");
		mips_list.push_back("syscall");
	}

	void write_expr(ValueType type, vector<string>& mips_list) {
		// 表达式可能是立即数，var,temp，const
		if (type == ValueType::INTV) {
			mips_list.push_back("li $v0, 1");
		} else { // char
			mips_list.push_back("li $v0, 11");
		}
		mips_list.push_back("syscall");

	}

	void write_lf(vector<string>& mips_list) {
		mips_list.push_back("la $v0, 11");
		mips_list.push_back("la $a0, \'\\n\'");
		mips_list.push_back("syscall");
	}

}


