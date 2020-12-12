#include "ImCoder.h"
#include"tools.h"


vector<shared_ptr<Quaternion>>& Function::get_quater_list() {
	
	return quater_list_;
}

shared_ptr<BasicBlock> Function::getBBlock(shared_ptr<Quaternion> quater) {
	return quater_bblock_map_[quater];
}

void Function::divide_bblock() {
	// 求入口语句： 函数入口，跳转后的第一条，跳转到的第一条(label)，return
	unordered_set<shared_ptr<Quaternion>> enter_set;
	for (auto it = quater_list_.begin(); it != quater_list_.end(); it++) {
		auto qtype = (*it)->quater_type_;
		if (qtype == QuaternionType::FuncDeclareHead) {
			enter_set.insert(*it);
		} else if (is_con_jump(qtype) || is_uncon_jump(qtype)) {
			enter_set.insert(*(it + 1));
		} else if (qtype == QuaternionType::Label) {
			enter_set.insert(*it);
		} else if (qtype == QuaternionType::FuncReturn) {
			if (it + 1 != quater_list_.end()) {
				enter_set.insert(*(it + 1));
			}
		}
	}

	// 划分基本块
	shared_ptr<BasicBlock> curr_bblock;
	for (auto quater : quater_list_) {
		if (enter_set.find(quater) != enter_set.end()) {
			curr_bblock = make_shared<BasicBlock>();
			this->bblock_list_.push_back()
		}
		
	}

	// 组织流图
}

void Function::active_analysis() {
}

bool is_con_jump(QuaternionType type) {
	return type == QuaternionType::BEQ || type == QuaternionType::BNE
		|| type == QuaternionType::BGT || type == QuaternionType::BGE
		|| type == QuaternionType::BLT || type == QuaternionType::BLE;
}

bool is_uncon_jump(QuaternionType type) {
	return type == QuaternionType::Goto;
}

/**********************************************************************************/

IMCode::IMCode() {
}

IMCode::~IMCode() {
}

void IMCode::addFunc(string& func) {
	if (strcmp_wo_case(func, "main")) {
		main_ = make_shared<Function>(func);
		curr_func_ = main_;
	} else {
		curr_func_ = make_shared<Function>(func);
		func_list_.push_back(curr_func_);
	}
}

//void IMCode::addBBlock(shared_ptr<BasicBlock> bblock) {
//	curr_func_->addBBlock(bblock);
//	curr_bblock_ = bblock;
//}

void IMCode::addQuater(shared_ptr<Quaternion> quater) {
	if (!quater) {  // for debug
		return;
	}
	if (!curr_func_) {
		gb_list_.push_back(quater);
	} else {
		curr_func_->addQuater(quater);
	}

}



vector<shared_ptr<Quaternion>>& IMCode::get_quater_list() {
	if (quater_list_.size() == 0) {
		// global var and const decalre
		quater_list_.insert(quater_list_.end(), gb_list_.begin(), gb_list_.end());
		// main
		const auto& main_list = this->main_->get_quater_list();
		quater_list_.insert(quater_list_.end(), main_list.begin(), main_list.end());
		// 非main函数
		for (auto func : this->func_list_) {
			const auto& func_list = func->get_quater_list();
			quater_list_.insert(quater_list_.end(), func_list.begin(), func_list.end());
		}
	}
	return quater_list_;
}

void IMCode::show_quaters(ostream& fout) {
	//cout << string(80, '_') << endl;
	for (auto quater : this->get_quater_list()) {
		if (quater) {
			fout << quater->toString() << endl;
			//cout << quater->toString() << endl;
		} else {
			fout << endl;
		}
	}
}

void IMCode::divide_bblock() {
	for (auto func : func_list_) {
		func->divide_bblock();
	}
}

void IMCode::active_analysis() {
	for (auto func : func_list_) {
		func->active_analysis();
	}
}




