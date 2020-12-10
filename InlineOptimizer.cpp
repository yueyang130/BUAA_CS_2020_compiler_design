#include "InlineOptimizer.h"
#include<assert.h>
#include"tools.h"
#include<map>

InlineOptimizer::InlineOptimizer(IMCode imcode): Optimizer("inline_opt", imcode) {
	for (auto caller : imcode_.func_list()) {
		if (checkInline(caller.get())) {
			inlined_func.insert(caller.get());
		}
	}
	optimize();
}

void InlineOptimizer::optimize() {
	// copyȫ������
	for (auto x : imcode_.global_list()) {
		optmized_imcode_.addQuater(x);
	}
	// ����
	for (auto caller : imcode_.func_list()) {
		// ���caller��Ҫ������,��caller���ᱻ��ӵ�optimized_imcode��func_list��
		if (inlined_func.find(caller.get()) == inlined_func.end()) {
			inline_caller(caller.get());
		}
	}
	inline_caller(&imcode_.main());
}

/*���caller������ÿһ����Ԫʽ������Ǻ������ã���������*/
void InlineOptimizer::inline_caller(Function* caller) {
	// �洢���������caller�����м����
	vector<shared_ptr<Quaternion>> inlined_quaters;
	auto func_quater_list = caller->get_quater_list();
	for (auto it = func_quater_list.begin(); it != func_quater_list.end(); it++) {
		// ����Ƿ��Ǻ�������
		if ((*it)->quater_type_ != QuaternionType::FuncCall) {
			inlined_quaters.push_back(*it);
			continue;
		}
		auto callee_entry = dynamic_pointer_cast<FunctionEntry>((*it)->opA_);
		auto callee = findFunction(this->imcode_, callee_entry->identifier());
		// ���callee�Ƿ���������ʸ�
		if (inlined_func.find(callee.get()) == inlined_func.end()) {
			inlined_quaters.push_back(*it);
			continue;
		}
		auto ret = do_inline(callee.get(), callee_entry->formal_param_num(), callee_entry->value_type(), inlined_quaters);
		// �з���ֵʱ����retAssgin��Ϊassign
		if (callee_entry->value_type() != ValueType::VOIDV) {
			it++;
			inlined_quaters.push_back(QuaternionFactory::Assign((*it)->result_, ret));
		}
	}
	// ����������м�ʽ�����µ�imcode
	string name = caller->name();
	optmized_imcode_.addFunc(name);
	for (auto x : inlined_quaters) {
		optmized_imcode_.addQuater(x);
	}
}

/*��callee������caller*/
shared_ptr<TableEntry> do_inline(Function* callee, int param_num, ValueType value_type, vector<shared_ptr<Quaternion>>& quater_list) {
	static int label_cnt = 0;
	string name = callee->name();
	string suffix = string("_" + name + to_string(++label_cnt));
	// callee�е�var��param �� caller��var��ӳ��
	map<TableEntry*, shared_ptr<TableEntry>> var_map;
	auto callee_quaters = callee->get_quater_list();
	vector<shared_ptr<TableEntry>> param_entrys;
	auto ret = make_shared<TempEntry>("ret_", value_type);
	auto label = make_shared<LabelEntry>(new_label());

	// step0: ɨ��һ��callee��Ԫʽ������ӳ��
	for (auto x : callee_quaters) {
		if (x->quater_type_ == QuaternionType::FuncFormalParam || x->quater_type_ == QuaternionType::VarDeclare) {
			auto var = x->result_;
			auto new_var = make_shared<VarEntry>(var->value_type(), var->identifier() + suffix, false);
			var_map[var.get()] = new_var;
		}
	}

	// step1: ��param push��Ϊassign
	int cnt = 0;
	vector<shared_ptr<Quaternion>> assign_quaters;
	for (auto it = quater_list.end() - param_num; it != quater_list.end(); it++, cnt++) {
		assert((*it)->quater_type_ == QuaternionType::FuncParamPush);
		auto var = callee_quaters[1 + cnt]->result_;
		auto q = QuaternionFactory::Assign(var_map[var.get()], (*it)->opA_);
		assign_quaters.push_back(q);
	}
	quater_list.erase(quater_list.end() - param_num, quater_list.end());
	quater_list.insert(quater_list.end(), assign_quaters.begin(), assign_quaters.end());

	vector<shared_ptr<Quaternion>> var_quaters;
	for (auto x : callee_quaters) {
		// ��������ͷ
		if (x->quater_type_ == QuaternionType::FuncDeclareHead) continue;
		// step2: ��callee�ľֲ������Ͳ���������Ϊcaller�ľֲ�����
		if (x->quater_type_ == QuaternionType::FuncFormalParam || x->quater_type_ == QuaternionType::VarDeclare) {
			auto var = x->result_;
			var_quaters.push_back(QuaternionFactory::VarDecalre(var_map[var.get()]));
			continue;
		} 
		// ���õ�Var��param���滻Ϊcaller��var
		auto new_quater = changeVarQuater(x, var_map);
		if (new_quater->quater_type_ == QuaternionType::FuncReturn) {
			// step3: ��return��Ϊassign + j 
			if (value_type != ValueType::VOIDV) {
				quater_list.push_back(QuaternionFactory::Assign(ret, new_quater->result_));
			}
			quater_list.push_back(QuaternionFactory::Goto(label));
		} else if (containLabel(new_quater)) {
			// ������ǩ����
			auto new_quater2 = changeLabel(new_quater, new_quater->result_->identifier() + suffix);
			quater_list.push_back(new_quater2);
		} else {
			quater_list.push_back(new_quater);
		}
	}
	quater_list.insert(quater_list.begin()+1, var_quaters.begin(), var_quaters.end());
	// set exit label
	quater_list.push_back(QuaternionFactory::Label(label));

	
	return ret;
}

bool checkInline(Function* callee) {
	if (callee->get_quater_list().size() > MAX_INLINE_ROW)
		return false;
	// ����Ƿ��Ե���
	for (auto quater : callee->get_quater_list()) {
		if (quater->quater_type_ == QuaternionType::FuncCall) {
			if (quater->opA_->identifier() == callee->name()) {
				return false;
			}
		}
	}
	return true;

}

shared_ptr<Function> findFunction(IMCode imcode,string func_name) {
	for (auto x : imcode.func_list()) {
		if (x->name() == func_name) {
			return x;
		}
	}
	return nullptr;
}

shared_ptr<Quaternion> changeVarQuater(shared_ptr<Quaternion> q0, map<TableEntry*, shared_ptr<TableEntry>>& var_map) {
	auto result = q0->result_;
	auto opA = q0->opA_;
	auto opB = q0->opB_;
	return make_shared<Quaternion>(q0->quater_type_, replaceVar(result, var_map),
		replaceVar(opA, var_map), replaceVar(opB, var_map));
}

shared_ptr<TableEntry> replaceVar(shared_ptr<TableEntry> entry, map<TableEntry*, shared_ptr<TableEntry>>& var_map) {
	if (!entry) {
		return nullptr;
	}
	// ȫ�ֱ�������
	if (entry->entry_type() != EntryType::VAR) {
		return entry;
	}
	bool global = dynamic_pointer_cast<VarEntry>(entry)->isGlobal();
	if (!global) {
		return var_map[entry.get()];
	} else {
		return entry;
	}
}

int containLabel(shared_ptr<Quaternion> quater) {
	auto type = quater->quater_type_;
	if (type == QuaternionType::Goto || type == QuaternionType::Label)
		return 1;
	if (type == QuaternionType::BEQ || type == QuaternionType::BNE
		|| type == QuaternionType::BLE || type == QuaternionType::BLT
		|| type == QuaternionType::BGE || type == QuaternionType::BGT)
		return 2;
	return 0;
}

shared_ptr<Quaternion> changeLabel(shared_ptr<Quaternion> q0, string new_lbl) {
	auto new_entry = make_shared<LabelEntry>(new_lbl);
	shared_ptr<Quaternion> new_quater = make_shared<Quaternion>(q0->quater_type_,
		q0->result_, q0->opA_, q0->opB_);
	new_quater->result_ = new_entry;
	return new_quater;
}




