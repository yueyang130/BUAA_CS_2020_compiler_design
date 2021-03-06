#include"opt_mips_code_tools.h"
#include<assert.h>
#include<sstream>
#include"tools.h"
#include<set>

namespace OptMips {

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

	void load_strcon(ImmediateEntry& inum, vector<string>& instr_list) {
		assert(inum.value_type() == ValueType::STRINGV);
		//vector<string> instrs;
		static set<ImmediateEntry*> str_set;
		// 函数内联后，可能会有一些重复的字符串entry
		if (str_set.find(&inum) == str_set.end() ) {
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
			instr_list.push_back(ret);
			str_set.insert(&inum);
			
		}

		
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

	void off_in_array(string reg0, string reg1, shared_ptr<TableEntry> entry, vector<shared_ptr<TableEntry>> idxs, vector<string>& mips_list,
		OptMipsFunctionGenerator& func_ ,RegisterPool& pool_) {
		auto arr = dynamic_pointer_cast<VarEntry>(entry);
		// 如果index是常数，直接计算； 如果是临时变量或局部变量，加载到寄存器计算

		if (arr->ndim() == 2) {
			// 加载第一个偏移
			if (const_or_immed(idxs[0].get())) {
				int off1 = getValue(idxs[0]->getValue()) * arr->getDimByte(0);
				mips_load_num(reg1, to_string(off1), mips_list);
			} else {
				string r_reg = pool_.assign_reg(idxs[0]);
				mips_alui(reg1, r_reg, to_string(arr->getDimByte(0)), QuaternionType::MulOp, mips_list);
			}
			// 加载第二个偏移
			if (const_or_immed(idxs[1].get())) {
				int off2 = getValue(idxs[1]->getValue()) * arr->getDimByte(1);
				mips_load_num(reg0, to_string(off2), mips_list);
			} else {
				string r_reg = pool_.assign_reg(idxs[1]);
				mips_alui(reg0, r_reg, to_string(arr->getDimByte(1)), QuaternionType::MulOp, mips_list);
			}
			mips_alu(reg0, reg0, reg1, QuaternionType::AddOp, mips_list);

		} else {
			if (const_or_immed(idxs[0].get())) {
				int off1 = getValue(idxs[0]->getValue()) * arr->getDimByte(0);
				mips_load_num(reg0, to_string(off1), mips_list);
			} else {
				string r_reg = pool_.assign_reg(idxs[0]);
				mips_alui(reg0, r_reg, to_string(arr->getDimByte(0)), QuaternionType::MulOp, mips_list);
			}
		}

		//if (idxs.size() == 1 && (result->entry_type() == EntryType::IMMEDIATE
		//	|| result->entry_type() == EntryType::CONST) ) {
		//	// 两个索引都是立即数或常数的情况
		//	load_var(result, buf_reg2);
		//	idxs.push_back(buf_reg2);
		//} else {
		//	string treg = reg_pool_.assign_reg(result);
		//	idxs.push_back(treg);
		//}
	}


	void mips_alu(string result, string left, string right, QuaternionType quater_type, vector<string>& mips_list) {

		string instr;
		switch (quater_type) {
		case QuaternionType::AddOp:
			instr = "addu " + result + ", " + left + ", " + right;
			mips_list.push_back(instr);
			break;
		case QuaternionType::SubOp:
		case QuaternionType::Neg:
			instr = "subu " + result + ", " + left + ", " + right;
			mips_list.push_back(instr);
			break;
		case QuaternionType::MulOp:
			instr = "mul " + result + ", " + left + ", " + right;
			mips_list.push_back(instr);
			break;
		case QuaternionType::DivOp:
			instr = "div " + left + ", " + right;
			mips_list.push_back(instr);
			mips_list.push_back("mflo " + result);
			break;
		default:
			break;
		}
	}

	void mips_alui(string result, string left, string immed_str, QuaternionType quater_type, vector<string>& mips_list) {
		static int label_cnt = 0;
		int immed = getValue(immed_str);
		string instr;
		switch (quater_type) {
		case QuaternionType::AddOp:
			instr = "addiu " + result + ", " + left + ", " + immed_str;
			mips_list.push_back(instr);
			break;
		case QuaternionType::SubOp:
		case QuaternionType::Neg:
				instr = "addiu " + result + ", " + left + ", " + to_string(-immed);
			mips_list.push_back(instr);
			break;
		case QuaternionType::MulOp:
		{
			int opB = immed;
			if (opB >= 0 && (opB & (opB - 1)) == 0) {
				int k = log2(opB);
				mips_list.push_back("sll " + result + ", " + left + ", " + to_string(k));

			} else if (opB < 0 && (-opB & (-opB - 1)) == 0) {
				int k = log2(-opB);
				mips_list.push_back("sll " + result + ", " + left + ", " + to_string(k));
				mips_alu(result, "$zero", result, QuaternionType::SubOp, mips_list);

			} else {
				instr = "mul " + result + ", " + left + ", " + immed_str;
				mips_list.push_back(instr);
			}
			break;
		}
		case QuaternionType::DivOp:
		{
			int opB = immed;
			bool neg = false;
			if (opB <= 0 && (-opB & (-opB - 1)) == 0){ // 2的整数次幂的负数
				opB = -opB;
				neg = true;
			}

			if (opB >= 0 && (opB & (opB - 1)) == 0) {  // 2的整数次幂
				// result = (x < 0 ? x + (1 << k) - 1 : x) >> k
				int k = log2(opB);
				// if left >=0, jump to label noNeg
				string label = "noNeg" + to_string(++label_cnt); 
				conditional_jump(left, "$zero", label, QuaternionType::BGE, mips_list);
				// else, x = x + (1 << k) - 1 
				mips_alui(left, left, to_string((1 << k) - 1), QuaternionType::AddOp, mips_list);
				// label noNeg:
				mips_list.push_back(label + ":");
				mips_list.push_back("sra " + result + ", " + left + ", " + to_string(k));
				if (neg) {
					mips_alu(result, "$zero", result, QuaternionType::SubOp, mips_list);
				}

			} else {
				mips_list.push_back("li $at, " + immed_str);
				instr = "div " + left + ", $at";
				mips_list.push_back(instr);
				mips_list.push_back("mflo " + result);
			}

			break;
		}
		default:
			break;
		}
	}

	void mips_move(string result, string left, vector<string>& mips_list) {
		mips_list.push_back("move " + result + ", " + left);
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