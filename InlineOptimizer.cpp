#include "InlineOptimizer.h"
#include<assert.h>
#include"tools.h"
#include<map>

InlineOptimizer::InlineOptimizer(IMCode imcode): Optimizer("inline_opt", imcode) {
	
	optimize();
}

void InlineOptimizer::optimize() {
	// copy全局声明
	for (auto x : imcode_.global_list()) {
		optmized_imcode_.addQuater(x);
	}
	// 主体
	for (auto caller : imcode_.func_list()) {
		inline_caller(caller.get());
	}
	inline_caller(&imcode_.main());
}

/*检查caller函数的每一个四元式，如果是函数调用，尝试内联*/
void InlineOptimizer::inline_caller(Function* caller) {
	// 存储被内联后的caller函数中间代码
	vector<shared_ptr<Quaternion>> inlined_quaters;
	auto func_quater_list = caller->get_quater_list();
	for (auto it = func_quater_list.begin(); it != func_quater_list.end(); it++) {
		// 检查是否是函数调用
		if ((*it)->quater_type_ != QuaternionType::FuncCall) {
			inlined_quaters.push_back(*it);
			continue;
		}
		auto callee_entry = dynamic_pointer_cast<FunctionEntry>((*it)->opA_);
		//auto callee = findFunction(this->imcode_, callee_entry->identifier());
		// callee使用的是之前作为caller嵌入了内联函数的版本
		auto callee = findFunction(this->optmized_imcode_, callee_entry->identifier());
		// 检查callee是否具有被内联资格
		if (caller->name() == callee_entry->identifier() || !checkInline(callee.get())) {
			inlined_quaters.push_back(*it);
			continue;
		}
		auto ret = do_inline(callee.get(), callee_entry->formal_param_num(), callee_entry->value_type(), inlined_quaters);
		// 有返回值时，将retAssgin改为assign
		if (callee_entry->value_type() != ValueType::VOIDV) {
			it++;
			inlined_quaters.push_back(QuaternionFactory::Assign((*it)->result_, ret));
		}
	}
	// 将内联后的中间式放入新的imcode
	string name = caller->name();
	optmized_imcode_.addFunc(name);
	for (auto x : inlined_quaters) {
		optmized_imcode_.addQuater(x);
	}
}

/**将callee内联到caller
* 
* 对于callee函数的参数和变量的处理:
* 1.参数和变量都在caller的头部进行声明，为在栈上预留空间
* 2.每次进入内联代码时，参数都有assign quater进行赋值操作
* 3.callee变量的初始化不能在声明时进行，要每次进入内联代码时都要重新初始化
*/
shared_ptr<TableEntry> do_inline(Function* callee, int param_num, ValueType value_type, vector<shared_ptr<Quaternion>>& quater_list) {
	static int label_cnt = 0;
	string name = callee->name();
	string suffix = string("_" + name + to_string(++label_cnt));
	// callee中的var和param 到 caller的var的映射
	map<TableEntry*, shared_ptr<TableEntry>> var_map;
	auto callee_quaters = callee->get_quater_list();
	vector<shared_ptr<TableEntry>> param_entrys;
	auto ret = make_shared<TempEntry>("ret_", value_type);
	auto label = make_shared<LabelEntry>(new_label());

	// step0: 扫描一遍callee四元式，创建映射
	for (auto x : callee_quaters) {
		if (x->quater_type_ == QuaternionType::FuncFormalParam || x->quater_type_ == QuaternionType::VarDeclare) {
			auto var = dynamic_pointer_cast<VarEntry>(x->result_);
			auto new_var = make_shared<VarEntry>(*var);
			new_var->setIdentifier(var->identifier() + suffix);
			var_map[var.get()] = new_var;
		}
	}

	// step1: 将param push改为assign
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
		// 抛弃函数头
		if (x->quater_type_ == QuaternionType::FuncDeclareHead) continue;
		// step2: 将callee的局部变量和参数，声明为caller的局部变量
		if (x->quater_type_ == QuaternionType::FuncFormalParam || x->quater_type_ == QuaternionType::VarDeclare) {
			auto var = dynamic_pointer_cast<VarEntry>(x->result_);
			// 变量声明时无需初始化
			var_quaters.push_back(QuaternionFactory::VarDecalre(var_map[var.get()]));
			//var_quaters.push_back(QuaternionFactory::VarDecalre(var_map[var.get()], x->opA_));
			// 将带有初始化的变量声明改为赋值语句
			if (x->opA_ != nullptr) {
				quater_list.push_back(QuaternionFactory::InlineVarInit(var_map[var.get()], x->opA_));
			}
			continue;
		} 
		// 将用到Var和param的四元式替换为caller的var
		auto new_quater = changeVarQuater(x, var_map);
		if (new_quater->quater_type_ == QuaternionType::FuncReturn) {
			// step3: 将return改为assign + j 
			if (value_type != ValueType::VOIDV) {
				quater_list.push_back(QuaternionFactory::Assign(ret, new_quater->result_));
			}
			quater_list.push_back(QuaternionFactory::Goto(label));
		} else if (containLabel(new_quater)) {
			// 更换标签名称
			auto new_quater2 = changeLabel(new_quater, new_quater->result_->identifier() + suffix);
			quater_list.push_back(new_quater2);
		} else {
			quater_list.push_back(new_quater);
		}
	}
	// 变量声明要插入到caller参数声明的后面
	auto it = quater_list.begin() + 1;
	for (; (*it)->quater_type_ == QuaternionType::FuncFormalParam;) {
		it++;
	}
	quater_list.insert(it, var_quaters.begin(), var_quaters.end());
	// set exit label
	quater_list.push_back(QuaternionFactory::Label(label));

	
	return ret;
}

bool checkInline(Function* callee) {
	if (callee->get_quater_list().size() > MAX_INLINE_ROW)
		return false;
	// 检查是否自调用
	for (auto quater : callee->get_quater_list()) {
		if (quater->quater_type_ == QuaternionType::FuncCall) {
			if (quater->opA_->identifier() == callee->name()) {
				return false;
			}
		}
	}
	return true;

}

shared_ptr<Function> findFunction(IMCode& imcode,string func_name) {
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
	// 全局变量不变
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




