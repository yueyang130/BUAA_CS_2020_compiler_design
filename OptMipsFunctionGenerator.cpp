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
							string treg = reg_pool_.assign_temp_reg(value);
							mips_store(treg, "$sp", -offset + a_off, value_type, mips_list_);
							a_off += unit_byte;
						}
					} else {
						// TODO:��ʱ���ں����ľֲ���������д���ڴ�ռ�
						// ������ȫ�ּĴ������䣬�ٸ����Ƿ���ȫ�ּĴ���
						string treg = reg_pool_.assign_temp_reg(inum->getValue());
						mips_store(treg, "$sp", -offset, value_type, mips_list_);
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


	void OptMipsFunctionGenerator::write_back(shared_ptr<TableEntry> var, string reg) {
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

		cout << var->identifier() <<" can not be found in global and local var map";
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


	OptMipsFunctionGenerator::OptMipsFunctionGenerator(Function& func, map<VarEntry*, int>& gb_var_map, vector<string>& mips_list
		,RegisterPool& reg_pool, IMCode& im_code) :
		func_(func), global_var_offset_map_(gb_var_map), mips_list_(mips_list), reg_pool_(reg_pool), im_coder_(im_code) {

		// �����Ӻ���ʱ,�Ѿ���ջ�Ĳ�������
		int stack_param_cnt = 0;
		// ���뺯��ʱʹ�õĲ���������ͳ���βθ���(�ββ�����Ƕ��)
		int formal_param_cnt = 0;
		// ���뺯��ʱʹ�õĲ��������������ܸ���
		int param_num = 0;
		// ���������±�ļĴ���
		vector<string> reg_idxs;

		reg_pool_.func_generator_ = this;

		auto& quater_list = this->func_.get_quater_list();
		for (auto it = quater_list.begin(); it != quater_list.end(); it++) {
			auto quater = *it;
			// �������һ���µĺ��������ߺ����ڿ�Խ�����飬�����ʱ�Ĵ�����
			if (it == quater_list.begin() || func_.getBBlock(*it) != func_.getBBlock(*(it - 1))) {
				reg_pool_.clearTempRegs(func_.getBBlock(*it)->active_in_);
			}

			if (!quater) { continue; }
			QuaternionType quater_type = quater->quater_type_;
			auto result = quater->result_;
			auto opA = quater->opA_;
			auto opB = quater->opB_;


			switch (quater_type) {
				// TODO: �Ծֲ�������Ŀǰ���������ʱ�Ĵ�����֮��Ӧ��Ϊ����ȫ�ּĴ���
			case InlineVarInit:
			{
				auto var = dynamic_pointer_cast<VarEntry>(result);
				auto inum = dynamic_pointer_cast<ImmediateEntry>(opA);
				if (var->isArray()) {
					int offset = this->func_var_offset_map_.at(var.get());
					int a_off = 0;
					for (string value : inum->initializingList()) {
						// �������ʼֵ���浽�ڴ�ռ�
						string treg = reg_pool_.assign_temp_reg(value);
						mips_store(treg, "$fp", offset + a_off, var->value_type(), mips_list_);
						a_off += (var->value_type() == ValueType::INTV) ? 4 : 1;
					}
				} else {
					// �˴����ص�����������Ϊ�ֲ�����������ȫ�ּĴ���
					string treg = reg_pool_.assign_temp_reg(inum->getValue());
					write_back(var, treg);
				}
				break;
			}
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
				mips_alui("$sp", "$sp", to_string(this->offset), QuaternionType::SubOp, mips_list_);
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
					string treg = reg_pool_.assign_temp_reg(result);
					mips_move("$v0", treg, mips_list);
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
				// TODO
				if (stack_param_cnt < 3) {
					/*this->load_var(opA, "$a" + to_string(stack_param_cnt + 1));
					stack_param_cnt++;*/
					string treg = reg_pool_.assign_temp_reg(opA);
					mips_store(treg, "$sp", -4 * (++stack_param_cnt), opA->value_type(), mips_list_);
				} else {
					string treg = reg_pool_.assign_temp_reg(opA);
					mips_store(treg, "$sp", -4 * (++stack_param_cnt), opA->value_type(), mips_list_);
				}
				break;
			case FuncCall:
			{
				// TODO: ��������ǰ�������������Ĵ���
				mips_alui("$sp", "$sp", to_string(stack_param_cnt * 4), QuaternionType::SubOp, mips_list_);
				mips_jal(opA->identifier(), mips_list);
				mips_alui("$sp", "$sp", to_string(stack_param_cnt * 4), QuaternionType::AddOp, mips_list_);
				// ���ں���Ƕ�׵��õ��������Ӧ�ö�actual_param_cnt���㣬Ӧ�ü�ȥ�Ѿ�ʹ�õ��˵Ĳ�������
				//actual_param_cnt = 0;
				int pnum = dynamic_pointer_cast<FunctionEntry>(opA)->formal_param_num();
				stack_param_cnt -= pnum;
				break;
			}
			case RetAssign:
				this->write_back(result, "$v0");
				break;
			case BEQ: case BNE: case BLT:
			case BLE: case BGT: case BGE:
			{
				string treg1 = reg_pool_.assign_temp_reg(opA);
				string treg2 = reg_pool_.assign_temp_reg(opB);
				conditional_jump(treg1, treg2, result->identifier(), quater_type, mips_list_);
				break;
			}
			case Goto:
				mips_j(*quater, mips_list_);
				break;
			case Label:
				set_label(*quater, mips_list_);
				break;
			case PushArrayIndex:
			{
				string treg = reg_pool_.assign_temp_reg(result);
				reg_idxs.push_back(treg);
				break;
			}
			case GetArrayElem:
			{
				string treg1 = reg_pool_.assign_temp_reg();
				string treg2 = reg_pool_.assign_temp_reg();
				off_in_array(treg1, treg2, opA, reg_idxs, mips_list_);
				reg_idxs.clear();
				string treg3 = reg_pool_.assign_temp_reg(result, false);
				this->load_array(opA, treg1, treg3);
				break;
			}
			case SetArrayELem:
			{
				string treg1 = reg_pool_.assign_temp_reg();
				string treg2 = reg_pool_.assign_temp_reg();
				off_in_array(treg1, treg2, result, reg_idxs, mips_list_);
				reg_idxs.clear();
				string treg3 = reg_pool_.assign_temp_reg(opA);
				this->store_array(result, treg1, treg3);
				break;
			}
			case AddOp: case SubOp:
			case MulOp: case DivOp:
			{
				// �������������ϲ���opA,opB����ͬʱΪ������������
				// 2+a, 2*a���ֱ�Ϊa+2, a*2, ���Խ�ʡ������������ָ���������λ����
				// ��2-a, 2/a�Ͳ��ܽ���λ�ã�ֻ��������������
				if (const_or_immed(opA.get()) && (quater_type == AddOp || quater_type == MulOp)) {
					auto temp = opA;
					opA = opB;
					opB = temp;
				}
				// Ȼ�������ʱopB������������alui��������
				if (const_or_immed(opB.get())) {
					string treg1 = reg_pool_.assign_temp_reg(opA);
					string treg2 = reg_pool_.assign_temp_reg(opB, false);
					mips_alui(treg2, treg1, opB->getValue(), quater_type, mips_list_);

				} else {
					string treg1 = reg_pool_.assign_temp_reg(opA);
					string treg2 = reg_pool_.assign_temp_reg(opB);
					string treg3 = reg_pool_.assign_temp_reg(opB, false);
					mips_alu(treg3, treg1, treg2, quater_type, mips_list_);
				}
				break;
			}
			case Neg:
				if (const_or_immed(opA.get())) {
					string treg1 = reg_pool_.assign_temp_reg(result, false);
					mips_alui(treg1, "$zero", opA->getValue(), quater_type, mips_list_);
				} else {
					string treg1 = reg_pool_.assign_temp_reg(opA);
					string treg2 = reg_pool_.assign_temp_reg(result, false);
					mips_alu(treg2, "$zero", treg1, quater_type, mips_list_);
				}
				break;

			case Assign:
			{
				string treg1 = reg_pool_.assign_temp_reg(opA);
				string treg2 = reg_pool_.assign_temp_reg(result, false);
				mips_alu(treg2, treg1, "$zero", QuaternionType::AddOp, mips_list_);
				break;
			}
			case Read:
			{
				read(result->value_type(), mips_list_);
				string treg = reg_pool_.assign_temp_reg(result, false);
				mips_alu(treg, "$v0", "$zero", QuaternionType::AddOp, mips_list_);
				/* ��gui��������������ַ���Ļس������ǣ������л�����ַ���Ļس�
				// ����Ƕ�ȡ�ַ�������Ҫ��ȡ�س�
				if (result->value_type() == ValueType::CHARV) {
					read(ValueType::CHARV, mips_list_);
				}
				*/
				break;
			}
			case Write:
			{
				if (opA.get()) {
					write_str(opA->identifier(), mips_list_);
				}
				if (opB.get()) {
					string treg = reg_pool_.assign_temp_reg(opB);
					mips_alu("$a0", treg, "$zero", QuaternionType::AddOp, mips_list_);
					write_expr(opB->value_type(), mips_list_);
				}
				write_lf(mips_list_);
				break;
			}
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

