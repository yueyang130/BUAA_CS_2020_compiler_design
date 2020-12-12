#include "ImCoder.h"
#include"tools.h"
#include<fstream>

vector<shared_ptr<Quaternion>>& Function::get_quater_list() {
	
	return quater_list_;
}

shared_ptr<BasicBlock> Function::getBBlock(shared_ptr<Quaternion> quater) {
	return quater_bblock_map_[quater];
}

void Function::divide_bblock() {
	// 求入口语句： 函数入口，跳转后的第一条，跳转到的第一条(label)，return的下一条
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
			this->bblock_list_.push_back(curr_bblock);
		}
		curr_bblock->addQuater(quater);
		quater_bblock_map_[quater] = curr_bblock;
	}

	// 组织流图: 无条件跳转和return都有一个后继块，有条件跳转有两个,其他顺序语句有一个后继块
	for (auto bblock : bblock_list_) {
		auto last_quater = bblock->get_quater_list().back();
		auto qtype = last_quater->quater_type_;
		if (is_uncon_jump(qtype)) {
			string label = last_quater->result_->identifier();
			auto q = findLabelQuater(label, quater_list_);
			auto b = quater_bblock_map_[q];
			bblock->addNextBlock(b);
			b->addPrevBlock(bblock);

		} else if(is_con_jump(qtype)) {
			string label = last_quater->result_->identifier();
			auto q1 = findNextQuater(last_quater, quater_list_);
			auto q2 = findLabelQuater(label, quater_list_);
			auto b1 = quater_bblock_map_[q1];
			auto b2 = quater_bblock_map_[q2];
			bblock->addNextBlock(b1);
			bblock->addNextBlock(b2);
			b1->addPrevBlock(bblock);
			b2->addPrevBlock(bblock);

		} else if (qtype == QuaternionType::FuncReturn) {
			// 排除return是整个函数最后一条语句的情况
			if (last_quater != quater_list_.back()) {
				auto q = findNextQuater(last_quater, quater_list_);
				auto b = quater_bblock_map_[q];
				bblock->addNextBlock(b);
				b->addPrevBlock(bblock);
			}
		} else {
			// 普通的顺序执行语句
			auto q = findNextQuater(last_quater, quater_list_);
			auto b = quater_bblock_map_[q];
			bblock->addNextBlock(b);
			b->addPrevBlock(bblock);
		}
	}

}

void Function::active_analysis() {
	// 基本块计算use和def集合
	for (auto bblock : bblock_list_) {
		bblock->cal_use_and_def();
	}

	bool unchange = true;
	do{
		unchange = true;
		for (auto it = bblock_list_.rbegin(); it != bblock_list_.rend(); it++) {
			unchange = unchange && (*it)->active_analysis();
		}
	} while (!unchange);
	
}

void Function::print_bblock(ofstream& fout) {
	fout << this->name() << endl;
	int cnt = 0;
	for (auto x : bblock_list_) {
		fout << "Basic Block " << ++cnt << endl;
		x->print_bblock(fout);
	}
	fout << endl;
	fout << endl;
}

bool is_con_jump(QuaternionType type) {
	return type == QuaternionType::BEQ || type == QuaternionType::BNE
		|| type == QuaternionType::BGT || type == QuaternionType::BGE
		|| type == QuaternionType::BLT || type == QuaternionType::BLE;
}

bool is_uncon_jump(QuaternionType type) {
	return type == QuaternionType::Goto;
}

shared_ptr<Quaternion> findLabelQuater(string label, vector<shared_ptr<Quaternion>>& list) {
	for (auto quater : list) {
		if (quater->quater_type_ == QuaternionType::Label && quater->result_->identifier() == label) {
			return quater;
		}
	}
	return nullptr;
}

shared_ptr<Quaternion> findNextQuater(shared_ptr<Quaternion> q, vector<shared_ptr<Quaternion>>& list) {
	for (auto it = list.begin(); it != list.end(); it++) {
		if (*it  == q) {
			return *(it+1);
		}
	}
	return nullptr;
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
	main_->divide_bblock();
	for (auto func : func_list_) {
		func->divide_bblock();
	}
}

void IMCode::active_analysis() {
	main_->active_analysis();
	for (auto func : func_list_) {
		func->active_analysis();
	}
}

void IMCode::print_bblock() {
	ofstream fout("bblock.txt");
	main_->print_bblock(fout);
	for (auto x : func_list_) {
		x->print_bblock(fout);
	}
}






