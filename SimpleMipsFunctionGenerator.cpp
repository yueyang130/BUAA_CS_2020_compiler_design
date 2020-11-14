#include "SimpleMipsFunctionGenerator.h"

void SimpleMipsFunctionGenerator::map_local_var() {
	for (auto& quater : func_.get_quater_list()) {
		if (quater->quater_type_ == QuaternionType::VarDeclare) {
			auto entry = dynamic_pointer_cast<VarEntry>(quater->result_);
			func_var_offset_map_[entry] = offset;
			
			if (quater->opA_.get()) {  // 是否有初始化
				auto inum = dynamic_pointer_cast<ImmediateEntry>(quater->opA_);
				int unit_byte = (entry->value_type() == ValueType::INTV) ? 4 : 1;
				if (entry->isArray()) {
					int a_off = 0;
					for (string value : inum->initializingList()) {
						mips_load_num(reg0, value, mips_list_);
						mips_store(reg0, "$sp", -(offset + a_off), mips_list_);
						a_off += unit_byte;
					}
				} else {
					mips_load_num(reg0, inum->getValue(), mips_list_);
					mips_store(reg0, "$sp", -offset, mips_list_);
				}
			}
			// 无论有没有初始化，都要在偏移量中加上数组的字节大小
			// ,因此不能通过初始化循环来计算byte_size
			offset += entry->ByteSize();
		}
	}
}

void SimpleMipsFunctionGenerator::map_temp_var() {
	for (auto& quater : func_.get_quater_list()) {
		// 目前只考虑出现在左值的临时编变量
		auto entry = quater->result_;
		if (isTempVar(entry)) {
			func_var_offset_map_[entry] = offset;
			offset += (entry->value_type() == ValueType::INTV) ? 4 : 1;
		}
	}
}

void SimpleMipsFunctionGenerator::load_var(shared_ptr<TableEntry> var, string reg) {
	EntryType type = var->entry_type();
	if (type == EntryType::TEMP || type == EntryType::VAR) {
		auto it = func_var_offset_map_.find(var);
		if (it != func_var_offset_map_.end()) {
			int offset = it->second;
			mips_load_mem(reg, "$sp", -offset, mips_list_);
			return;
		}

		auto it2 = global_var_offset_map_.find(dynamic_pointer_cast<VarEntry>(var));
		if (it2 != global_var_offset_map_.end()) {
			int offset = it->second;
			mips_load_mem(reg, "$gp", offset, mips_list_);
			return;
		}
	}
	else if (type == EntryType::CONST || type == EntryType::IMMEDIATE) {
		string value = var->getValue();
		mips_load_num(reg, value, mips_list_);
	}
}

void SimpleMipsFunctionGenerator::store_var(shared_ptr<TableEntry> var, string reg) {
	auto it = func_var_offset_map_.find(var);
	if (it != func_var_offset_map_.end()) {
		int offset = it->second;
		mips_store(reg, "$sp", -offset, mips_list_);
		return;
	}

	auto it2 = global_var_offset_map_.find(dynamic_pointer_cast<VarEntry>(var));
	if (it2 != global_var_offset_map_.end()) {
		int offset = it->second;
		mips_store(reg, "$gp", offset, mips_list_);
		return;
	}

	exit(1);

}


SimpleMipsFunctionGenerator::SimpleMipsFunctionGenerator(Function& func, map<shared_ptr<VarEntry>, int>& gb_var_map, vector<string>& mips_list) :
	func_(func), global_var_offset_map_(gb_var_map), mips_list_(mips_list)
{
	map_local_var();
	map_temp_var();

	for (auto& quater : func_.get_quater_list()) {
		QuaternionType quater_type = quater->quater_type_;
		auto result = quater->result_;
		auto opA = quater->opA_;
		auto opB = quater->opB_;

		switch (quater_type) {
		case FuncDeclareHead:
			//set_label(*quater, mips_list_);
			break;
		case FuncFormalParam:
			break;
		case FuncReturn:
			break;
		case FuncParamPush:
			break;
		case FuncCall:
			break;
		case RetAssign:
			break;
		case BEQ: case BNE: case BLT:
		case BLE: case BGT: case BGE:
			this->load_var(opA, reg1);
			this->load_var(opB, reg2);
			alu(reg0, reg1, reg2, quater_type, mips_list_);
			this->store_var(result, reg0);
			break;
		case Goto:
			j(*quater, mips_list_);
			break;
		case Label:
			set_label(*quater, mips_list_);
			break;
		case GetArrayElem:
			break;
		case SetArrayELem:
			break;
		case AddOp: case SubOp:
		case MulOp: case DivOp:
			this->load_var(opA, reg1);
			this->load_var(opB, reg2);
			alu(reg0, reg1, reg2, quater_type, mips_list_);
			this->store_var(result, reg0);
			break;

		case Neg:
			this->load_var(opB, reg2);
			alu(reg0, "$zero", reg2, quater_type, mips_list_);
			this->store_var(result, reg0);
			break;

		case Assign:
			this->load_var(opA, reg1);
			mips_load_reg(reg0, reg1, mips_list_);
			this->store_var(result, reg0);
			break;

		case Read:
			read(result->value_type(), mips_list_);
			this->store_var(result, "$v0");
			break;

		case Write:
			if (opA.get()) {
				write_str(opA->identifier(), mips_list_);
			}
			if (opB.get()) {
				this->load_var(opB, "$a0");
				write_expr(opB->value_type(), mips_list_);
			}
			write_lf(mips_list_);
			break;

		default:
			break;
		}
		
	}
	
}

bool isTempVar(shared_ptr<TableEntry> entry) {
	if (!entry.get()) {
		return false;
	}
	if (entry->entry_type() != EntryType::TEMP) {
		return false;
	}
	return true;
}

