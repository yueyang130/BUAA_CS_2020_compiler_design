#include "OptMipsFunctionGenerator.h"
#include "tools.h"


namespace OptMips {

	void OptMipsFunctionGenerator::map_local_var() {
		for (auto& quater : func_.get_quater_list()) {
			if (quater->quater_type_ == QuaternionType::VarDeclare) {
				auto entry = dynamic_pointer_cast<VarEntry>(quater->result_);
				ValueType value_type = entry->value_type();

				// ������û�г�ʼ������Ҫ��ƫ�����м���������ֽڴ�С
					// ,��˲���ͨ����ʼ��ѭ��������byte_size
				offset += entry->ByteSize();
				// intҪ�ֶ���
				if (entry->value_type() == ValueType::INTV) {
					int ret = offset % 4;
					if (ret != 0) {
						offset = offset - ret + 4;
					}
				}
				// �Ȱѵ�ǰ������size����offset����map var to offset
				func_var_offset_map_[entry.get()] = -offset;

				if (quater->opA_.get()) {  // �Ƿ��г�ʼ��
					auto inum = dynamic_pointer_cast<ImmediateEntry>(quater->opA_);
					int unit_byte = (entry->value_type() == ValueType::INTV) ? 4 : 1;

					if (entry->isArray()) {
						int a_off = 0;
						for (string value : inum->initializingList()) {
							mips_load_num(reg0, value, mips_list_);
							mips_store(reg0, "$sp", -offset + a_off, value_type, mips_list_);
							a_off += unit_byte;
						}
					} else {
						mips_load_num(reg0, inum->getValue(), mips_list_);
						mips_store(reg0, "$sp", -offset, value_type, mips_list_);
					}
				}


			}
		}
	}

	void OptMipsFunctionGenerator::map_temp_var() {
		for (auto& quater : func_.get_quater_list()) {
			// Ŀǰֻ���ǳ�������ֵ����ʱ�����
			auto entry = quater->result_;
			if (isTempVar(entry)) {
				offset += (entry->value_type() == ValueType::INTV) ? 4 : 1;
				if (entry->value_type() == ValueType::INTV) {
					int ret = offset % 4;
					if (ret != 0) {
						offset = offset - ret + 4;
					}
				}
				func_var_offset_map_[entry.get()] = -offset;
			}
		}
	}

	void OptMipsFunctionGenerator::load_var(shared_ptr<TableEntry> var, string reg) {
		EntryType entry_type = var->entry_type();
		ValueType value_type = var->value_type();
		if (entry_type == EntryType::TEMP || entry_type == EntryType::VAR) {
			auto it = func_var_offset_map_.find(var.get());
			if (it != func_var_offset_map_.end()) {
				int offset = it->second;
				mips_load_mem(reg, "$fp", offset, value_type, mips_list_);
				return;
			}

			auto it2 = global_var_offset_map_.find(dynamic_pointer_cast<VarEntry>(var).get());
			if (it2 != global_var_offset_map_.end()) {
				int offset = it2->second;
				mips_load_mem(reg, "$gp", offset, value_type, mips_list_);
				return;
			}
		} else if (entry_type == EntryType::CONST || entry_type == EntryType::IMMEDIATE) {
			string value = var->getValue();
			mips_load_num(reg, value, mips_list_);
		}
	}


	void OptMipsFunctionGenerator::store_var(shared_ptr<TableEntry> var, string reg) {
		ValueType value_type = var->value_type();
		auto it = func_var_offset_map_.find(var.get());
		if (it != func_var_offset_map_.end()) {
			int offset = it->second;
			mips_store(reg, "$fp", offset, value_type, mips_list_);
			return;
		}

		auto it2 = global_var_offset_map_.find(dynamic_pointer_cast<VarEntry>(var).get());
		if (it2 != global_var_offset_map_.end()) {
			int offset = it2->second;
			mips_store(reg, "$gp", offset, value_type, mips_list_);
			return;
		}

		cout << "the variable can not be found in global and local var map";
		exit(EXIT_FAILURE);

	}

	void OptMipsFunctionGenerator::load_array(shared_ptr<TableEntry> var, string off_reg, string target_reg) {
		ValueType value_type = var->value_type();
		auto it = func_var_offset_map_.find(var.get());
		if (it != func_var_offset_map_.end()) {
			int offset = it->second;  // ����ͷ�����fp��ƫ����
			mips_alu(off_reg, off_reg, "$fp", QuaternionType::AddOp, mips_list_);
			mips_load_mem(target_reg, off_reg, offset, value_type, mips_list_);
			return;
		}
		// global array
		mips_load_mem(target_reg, off_reg, var->identifier(), value_type, mips_list_);
		return;
	}

	void OptMipsFunctionGenerator::store_array(shared_ptr<TableEntry> var, string off_reg, string source_reg) {
		ValueType value_type = var->value_type();
		auto it = func_var_offset_map_.find(var.get());
		if (it != func_var_offset_map_.end()) {
			int offset = it->second;  // ����ͷ�����fp��ƫ����
			mips_alu(off_reg, off_reg, "$fp", QuaternionType::AddOp, mips_list_);
			mips_store(source_reg, off_reg, offset, value_type, mips_list_);
			return;
		}
		// global array
		mips_store(source_reg, off_reg, var->identifier(), value_type, mips_list_);
		return;

	}


	OptMipsFunctionGenerator::OptMipsFunctionGenerator(Function& func, map<VarEntry*, int>& gb_var_map, vector<string>& mips_list) :
		func_(func), global_var_offset_map_(gb_var_map), mips_list_(mips_list) {

		// �����Ӻ���ʱ,�Ѿ���ջ�Ĳ�������
		int stack_param_cnt = 0;
		// ���뺯��ʱʹ�õĲ���������ͳ���βθ���(�ββ�����Ƕ��)
		int formal_param_cnt = 0;
		// ���뺯��ʱʹ�õĲ��������������ܸ���
		int param_num = 0;
		// ���������±�ļĴ���
		vector<string> reg_idxs{ "$t0", "$t1" };
		int idx_cnt = 0;

		for (auto& quater : func_.get_quater_list()) {
			if (!quater) { continue; }
			QuaternionType quater_type = quater->quater_type_;
			auto result = quater->result_;
			auto opA = quater->opA_;
			auto opB = quater->opB_;


			switch (quater_type) {
			case FuncDeclareHead:
				set_label(*quater, mips_list_);
				param_num = dynamic_pointer_cast<FunctionEntry>(result)->formal_param_num();

				// save ra, previous fp, local var, temp var
				mips_store("$ra", "$sp", -4, ValueType::INTV, mips_list_);
				mips_store("$fp", "$sp", -8, ValueType::INTV, mips_list_);
				mips_load_reg("$fp", "$sp", mips_list_);
				this->offset += 8;
				map_local_var();
				map_temp_var();
				// ����offset
				offset = (offset % 4 == 0) ? offset : (offset - (offset % 4) + 4);
				mips_alu("$sp", "$sp", to_string(this->offset), QuaternionType::SubOp, mips_list_);
				break;
			case FuncFormalParam:
			{
				int offset = (param_num - (++formal_param_cnt)) * 4;
				func_var_offset_map_[result.get()] = offset;
				break;
			}
			case FuncReturn:
			{
				// set return value
				if (result) {
					this->load_var(result, "$v0");
				}
				// restore sp, fp, ra
				mips_load_mem("$ra", "$fp", -4, ValueType::INTV, mips_list_);
				mips_load_reg("$sp", "$fp", mips_list_);
				mips_load_mem("$fp", "$fp", -8, ValueType::INTV, mips_list_);

				bool ismain = strcmp_wo_case(func_.name(), "main");
				mips_jr(ismain, mips_list_);
				break;
			}
			case FuncParamPush:
				this->load_var(opA, reg0);
				mips_store(reg0, "$sp", -4 * (++stack_param_cnt), opA->value_type(), mips_list_);
				break;
			case FuncCall:
			{
				mips_alu("$sp", "$sp", to_string(stack_param_cnt * 4), QuaternionType::SubOp, mips_list_);
				mips_jal(opA->identifier(), mips_list);
				mips_alu("$sp", "$sp", to_string(stack_param_cnt * 4), QuaternionType::AddOp, mips_list_);
				// ���ں���Ƕ�׵��õ��������Ӧ�ö�actual_param_cnt���㣬Ӧ�ü�ȥ�Ѿ�ʹ�õ��˵Ĳ�������
				//actual_param_cnt = 0;
				int pnum = dynamic_pointer_cast<FunctionEntry>(opA)->formal_param_num();
				stack_param_cnt -= pnum;
				break;
			}
			case RetAssign:
				this->store_var(result, "$v0");
				break;
			case BEQ: case BNE: case BLT:
			case BLE: case BGT: case BGE:
				this->load_var(opA, reg1);
				this->load_var(opB, reg2);
				conditional_jump(reg1, reg2, result->identifier(), quater_type, mips_list_);
				break;
			case Goto:
				mips_j(*quater, mips_list_);
				break;
			case Label:
				set_label(*quater, mips_list_);
				break;
			case PushArrayIndex:
				this->load_var(result, reg_idxs[idx_cnt++]);
				break;

			case GetArrayElem:
				off_in_array(reg0, reg1, opA, reg_idxs, mips_list_);
				idx_cnt = 0;
				this->load_array(opA, reg0, reg1);
				this->store_var(result, reg1);
				break;
			case SetArrayELem:
				off_in_array(reg0, reg1, result, reg_idxs, mips_list_);
				idx_cnt = 0;
				this->load_var(opA, reg1);
				this->store_array(result, reg0, reg1);
				break;

			case AddOp: case SubOp:
			case MulOp: case DivOp:
				this->load_var(opA, reg1);
				this->load_var(opB, reg2);
				mips_alu(reg0, reg1, reg2, quater_type, mips_list_);
				this->store_var(result, reg0);
				break;

			case Neg:
				this->load_var(opA, reg2);
				mips_alu(reg0, "$zero", reg2, quater_type, mips_list_);
				this->store_var(result, reg0);
				break;

			case Assign:
				this->load_var(opA, reg0);
				this->store_var(result, reg0);
				break;

			case Read:
				read(result->value_type(), mips_list_);
				this->store_var(result, "$v0");
				/* ��gui��������������ַ���Ļس������ǣ������л�����ַ���Ļس�
				// ����Ƕ�ȡ�ַ�������Ҫ��ȡ�س�
				if (result->value_type() == ValueType::CHARV) {
					read(ValueType::CHARV, mips_list_);
				}
				*/
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

}

