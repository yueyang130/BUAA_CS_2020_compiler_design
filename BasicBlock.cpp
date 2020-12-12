#include "BasicBlock.h"
#include<fstream>

BasicBlock::BasicBlock() {
	quater_list_.reserve(128);
	
}

void BasicBlock::cal_use_and_def() {
	// 确定use和def集合
	for (auto quater : quater_list_) {
		auto qtype = quater->quater_type_;
		auto result = quater->result_;
		auto opA = quater->opA_;
		auto opB = quater->opB_;
		// 这里的def指被赋值
		switch (qtype) {
		case QuaternionType::FuncReturn:
			if(result) add_use_var(result);
			break;
		case QuaternionType::FuncParamPush:
			add_use_var(opA);
			break;
		case QuaternionType::RetAssign:
			add_def_var(result);
			break;
		case QuaternionType::VarDeclare:
			if (opA) {
				add_def_var(result);
			}
			break;
		case QuaternionType::InlineVarInit:
			add_def_var(result);
			break;
		case QuaternionType::BEQ: case QuaternionType::BNE:
		case QuaternionType::BLT: case QuaternionType::BLE:
		case QuaternionType::BGT: case QuaternionType::BGE:
			add_use_var(opA);
			add_use_var(opB);
			break;
		case QuaternionType::PushArrayIndex:
			add_use_var(result);
			break;
		case QuaternionType::GetArrayElem:
			add_def_var(result);
			break;
		case QuaternionType::SetArrayELem:
			add_use_var(opA);
			break;
		case QuaternionType::AddOp: case QuaternionType::SubOp:
		case QuaternionType::MulOp: case QuaternionType::DivOp:
			add_use_var(opA);
			add_use_var(opB);
			add_def_var(result);
			break;
		case QuaternionType::Neg:
			add_use_var(opA);
			add_def_var(result);
			break;
		case QuaternionType::Assign:
			add_use_var(opA);
			add_def_var(result);
			break;
		case QuaternionType::Read:
			add_def_var(result);
			break;
		case QuaternionType::Write:
			if (opA) add_use_var(opA);
			if (opB) add_use_var(opB);
			break;
		default:
			break;
		}
	}
}

bool BasicBlock::active_analysis() {
	// 计算out
	unordered_set<shared_ptr<TableEntry>> new_out;
	for (auto bblock : next_set_) {
		new_out.insert(bblock->active_in_.begin(), bblock->active_in_.end());
	}
	active_out_ = new_out;  
	// 计算in
	unordered_set<shared_ptr<TableEntry>> new_in;
	for (auto x : active_out_) {
		if (def_.find(x) == def_.end()) {
			new_in.insert(x);
		}
	}
	new_in.insert(use_.begin(),use_.end());
	bool unchange = new_in.size() == active_in_.size();
	active_in_ = new_in;
	return unchange;
}

void BasicBlock::print_bblock(ofstream& fout) {
	fout << "use set:" << endl;
	for (auto x : use_) {
		fout << x->identifier() << " ";
	}
	fout << endl;
	fout << "def set:" << endl;
	for (auto x : def_) {
		fout << x->identifier() << " ";
	}
	fout << endl;
	fout << "active in set:" << endl;
	for (auto x : active_in_) {
		fout << x->identifier() << " ";
	}
	fout << endl;
	fout << "active out set:" << endl;
	for (auto x : active_out_) {
		fout << x->identifier() << " ";
	}
	fout << endl;
	fout << "quater:" << endl;
	for (auto x : quater_list_) {
		fout << x->toString() << endl;
	}
	fout << endl;
}

void BasicBlock::add_use_var(shared_ptr<TableEntry> var) {
	auto entry_type = var->entry_type();
	if (entry_type == EntryType::TEMP || entry_type == EntryType::VAR) {
		if (def_.find(var) == def_.end()) {
			use_.insert(var);
		}
	}
	
}

void BasicBlock::add_def_var(shared_ptr<TableEntry> var) {
	auto entry_type = var->entry_type();
	if (entry_type == EntryType::TEMP || entry_type == EntryType::VAR) {
		if (use_.find(var) == use_.end()) {
			def_.insert(var);
		}
	}
}


