#include "OptMipsFunctionGenerator.h"
#include "tools.h"


namespace OptMips {



	unordered_map<shared_ptr<TableEntry>, int> OptMipsFunctionGenerator::count_ref() {
		unordered_map<shared_ptr<TableEntry>, int> ref_map;
		int LOOP_WEIGHT = 100;

		auto& quater_list = func_.get_quater_list();
		auto it = quater_list.begin();
		while (it != quater_list.end()) {
			auto result = (*it)->result_;
			auto opA = (*it)->opA_;
			auto opB = (*it)->opB_;
			// Ѱ��ѭ��
			if ((*it)->quater_type_ == QuaternionType::Label) {
				bool isloop = false;
				auto it2 = it+1;
				// Ѱ��ѭ���Ľ�����
				while (it2 != quater_list.end()) {
					if ((*it2)->quater_type_ == QuaternionType::Goto
						&& (*it2)->result_->identifier() == result->identifier()) {
						isloop = true;
						break;
					}
					it2++;
				}
				// ѭ����������
				if (isloop) {
					while (it != it2) {
						auto result = (*it)->result_;
						auto opA = (*it)->opA_;
						auto opB = (*it)->opB_;
						_count(result, ref_map, LOOP_WEIGHT);
						_count(opA, ref_map, LOOP_WEIGHT);
						_count(opB, ref_map, LOOP_WEIGHT);
						it++;
					}
				}

			} else {
				_count(result, ref_map);
				_count(opA, ref_map);
				_count(opB, ref_map);
			}
			it++;

		}
		return ref_map;
	}

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
							mips_load_num(buf_reg1, value, mips_list_);
							mips_store(buf_reg1, "$sp", -offset + a_off, value_type, mips_list_);
							a_off += unit_byte;
						}
					} else {

						reg_pool_.deal_var_decalre(entry, inum->getValue(), offset);
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
		 reg_pool_(reg_pool), func_(func), global_var_offset_map_(gb_var_map), mips_list_(mips_list) {

		// �����Ӻ���ʱ,�Ѿ���ջ�Ĳ�������
		int stack_param_cnt = 0;
		// ���뺯��ʱʹ�õĲ���������ͳ���βθ���(�ββ�����Ƕ��)
		int formal_param_cnt = 0;
		// ���뺯��ʱʹ�õĲ��������������ܸ���
		int param_num = 0;
		// ���������±�ļĴ���
		vector<shared_ptr<TableEntry>> idxs;
		// ���ڼĴ��������ı�־λ
		bool save_reg = true;
		// ���ڼ�¼��Щ�Ĵ�����Ҫ����
		vector<string> save_list;

		reg_pool_.func_generator_ = this;
		reg_pool_.count_var_ref(this->count_ref());
		

		auto& quater_list = this->func_.get_quater_list();
		for (auto it = quater_list.begin(); it != quater_list.end(); it++) {
			auto quater = *it;

			// �������һ���µĺ��������ߺ����ڿ�Խ�����飬�����ʱ�Ĵ�����
			// д��ʱ��Ӧ�÷������뿪��ǰ������ǰ�������ǽ�����һ���������
			// ������ԣ�������һ������ת����������תǰ�����������һ�����֮��
			shared_ptr<BasicBlock> bblock = nullptr;
	
			if (it == quater_list.end()-1 || func_.getBBlock(*it) != func_.getBBlock(*(it + 1))) {
				bblock = func_.getBBlock(*it);
			}
			auto qtype = quater->quater_type_;
			if (bblock) {
				if (is_con_jump(qtype) || is_uncon_jump(qtype) || qtype == QuaternionType::FuncReturn) {
					reg_pool_.treg_write_back(bblock->active_out_);
				}
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
						mips_load_num(buf_reg1, value, mips_list_);
						mips_store(buf_reg1, "$fp", offset + a_off, var->value_type(), mips_list_);
						a_off += (var->value_type() == ValueType::INTV) ? 4 : 1;
					}
				} else {
					reg_pool_.deal_var_decalre(var, inum->getValue(), offset);
				}
				break;
			}
			case FuncDeclareHead:
			{
				set_label(*quater, mips_list_);
				param_num = dynamic_pointer_cast<FunctionEntry>(result)->formal_param_num();

				// save ra, previous fp, local var, temp var
				mips_store("$ra", "$sp", -4, ValueType::INTV, mips_list_);
				mips_store("$fp", "$sp", -8, ValueType::INTV, mips_list_);
				mips_load_reg("$fp", "$sp", mips_list_);
				this->offset += 8;
				reg_pool_.load_gb_var();
				map_local_var();
				map_temp_var();
				// ����offset
				offset = (offset % 4 == 0) ? offset : (offset - (offset % 4) + 4);
				mips_alui("$sp", "$sp", to_string(this->offset), QuaternionType::SubOp, mips_list_);
				break;
			}
			case FuncFormalParam:
			{
				int offset = (param_num - (++formal_param_cnt)) * 4;
				func_var_offset_map_[result.get()] = offset;
				reg_pool.deal_param_decalre(result, offset);
				break;
			}
			case FuncReturn:
			{
				// set return value
				if (result) {
					string treg = reg_pool_.assign_reg(result);
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
			{
				if (stack_param_cnt < 3) {
					/*this->load_var(opA, "$a" + to_string(stack_param_cnt + 1));
					stack_param_cnt++;*/
					string treg = reg_pool_.assign_reg(opA);
					mips_store(treg, "$sp", -4 * (++stack_param_cnt + SAVE_SPACE), opA->value_type(), mips_list_);
				} else {
					string treg = reg_pool_.assign_reg(opA);
					mips_store(treg, "$sp", -4 * (++stack_param_cnt + SAVE_SPACE), opA->value_type(), mips_list_);
				}
				break;
			}
			case FuncCall:
			{
				// ����push�󣬺�������ǰ�������������Ĵ���
				// ���ܷ��ڲ���pushǰ����Ϊ�����Ļ����е�reg���ڲ���push��ʱ���load����û�б�������
				// ֱ����ջ�����˸���λ
				if (save_reg) {
					reg_pool_.save_tregs(save_list);
					save_reg = false;
				}
				mips_alui("$sp", "$sp", to_string((stack_param_cnt + SAVE_SPACE)* 4), QuaternionType::SubOp, mips_list_);
				mips_jal(opA->identifier(), mips_list);
				mips_alui("$sp", "$sp", to_string((stack_param_cnt + SAVE_SPACE) * 4), QuaternionType::AddOp, mips_list_);
				// ���ں���Ƕ�׵��õ��������Ӧ�ö�actual_param_cnt���㣬Ӧ�ü�ȥ�Ѿ�ʹ�õ��˵Ĳ�������
				//actual_param_cnt = 0;
				int pnum = dynamic_pointer_cast<FunctionEntry>(opA)->formal_param_num();
				stack_param_cnt -= pnum;
				// �ָ��ֳ�
				reg_pool_.restore_tregs(save_list);
				save_reg = true;
				break;
			}
			case RetAssign:
				this->write_back(result, "$v0");
				break;
			case BEQ: case BNE: case BLT:
			case BLE: case BGT: case BGE:
			{
				auto tregs = reg_pool_.assign_reg(opA, opB);
				string treg1 = tregs[0];
				string treg2 = tregs[1];
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
				idxs.push_back(result);
				//if (idxs.size() == 1 && (result->entry_type() == EntryType::IMMEDIATE
				//	|| result->entry_type() == EntryType::CONST) ) {
				//	// ���������������������������
				//	load_var(result, buf_reg2);
				//	idxs.push_back(buf_reg2);
				//} else {
				//	string treg = reg_pool_.assign_reg(result);
				//	idxs.push_back(treg);
				//}
				
				break;
			}
			case GetArrayElem:
			{
				off_in_array(buf_reg1, buf_reg2, opA, idxs, mips_list_, *this, reg_pool_);
				idxs.clear();
				string treg3 = reg_pool_.assign_reg(result, false);
				this->load_array(opA, buf_reg1, treg3);
				break;
			}
			case SetArrayELem:
			{
				off_in_array(buf_reg1, buf_reg2, result, idxs, mips_list_, *this, reg_pool_);
				idxs.clear();
				if (opA->entry_type() == IMMEDIATE || opA->entry_type() == EntryType::CONST) {
					load_var(opA, buf_reg2);
					this->store_array(result, buf_reg1, buf_reg2);
				} else {
					string treg3 = reg_pool_.assign_reg(opA);
					this->store_array(result, buf_reg1, treg3);
				}
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
					string treg1 = reg_pool_.assign_reg(opA);
					string treg2 = reg_pool_.assign_reg(result, false);
					mips_alui(treg2, treg1, opB->getValue(), quater_type, mips_list_);

				} else {
					string treg1 = reg_pool_.assign_reg(opA);
					string treg2 = reg_pool_.assign_reg(opB);
					string treg3 = reg_pool_.assign_reg(result, false);
					mips_alu(treg3, treg1, treg2, quater_type, mips_list_);
				}
				break;
			}
			case Neg:
				if (const_or_immed(opA.get())) {
					string treg1 = reg_pool_.assign_reg(result, false);
					mips_alui(treg1, "$zero", opA->getValue(), quater_type, mips_list_);
				} else {
					string treg1 = reg_pool_.assign_reg(opA);
					string treg2 = reg_pool_.assign_reg(result, false);
					mips_alu(treg2, "$zero", treg1, quater_type, mips_list_);
				}
				break;
				
			case Assign:
			{
				if (const_or_immed(opA.get())) {
					string treg2 = reg_pool_.assign_reg(result, false);
					mips_load_num(treg2, opA->getValue(), mips_list_);
				} else {
					string treg1 = reg_pool_.assign_reg(opA);
					string treg2 = reg_pool_.assign_reg(result, false);
					mips_move(treg2, treg1, mips_list_);
				}
				break;
			}
			case Read:
			{
				read(result->value_type(), mips_list_);
				string treg = reg_pool_.assign_reg(result, false);
				mips_move(treg, "$v0", mips_list_);
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
					string treg = reg_pool_.assign_reg(opB);
					mips_alu("$a0", treg, "$zero", QuaternionType::AddOp, mips_list_);
					write_expr(opB->value_type(), mips_list_);
				}
				write_lf(mips_list_);
				break;
			}
			default:
				break;
			}

			if (bblock) {
				if (!(is_con_jump(qtype) || is_uncon_jump(qtype) || qtype == QuaternionType::FuncReturn)) {
					reg_pool_.treg_write_back(bblock->active_out_);
				}
				// ͳһ�������ռĴ������ڴ���ӳ��
				reg_pool_.clearTempRegs();
			}


		}

		reg_pool_.clear_sreg();
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

	void _count(shared_ptr<TableEntry> entry, unordered_map<shared_ptr<TableEntry>, int>& ref_map, int w) {
		if (!entry) return;
		if (entry->entry_type() != EntryType::VAR) {
			return;
		}
		auto var = dynamic_pointer_cast<VarEntry>(entry);
		if (var->isArray()) return;
		// key����map��ʱ�Զ���ʼ��
		ref_map[entry] += w;

	}

}

