#include "BasicBlock.h"
#include"tools.h"


vector<shared_ptr<Quaternion>>& BasicBlock::get_quater_list() {
	return quater_list_;
}


const vector<shared_ptr<Quaternion>>& Function::get_quater_list() {
	if (this->quater_list_.size() == 0) {
		for (auto bblock : this->bblock_list_) {
			const auto& sub_list = bblock->get_quater_list();
			quater_list_.insert(quater_list_.end(), sub_list.begin(), sub_list.end());
		}
	}
	return quater_list_;
}


IMCode::IMCode() : global_bblock_(new BasicBlock()) {
	curr_bblock_ = global_bblock_;
}


IMCode& IMCode::getInstance() {
	static IMCode im_cooder;
	return im_cooder;
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

void IMCode::addBBlock(shared_ptr<BasicBlock> bblock) {
	curr_func_->addBBlock(bblock);
	curr_bblock_ = bblock;
}

void IMCode::addQuater(shared_ptr<Quaternion> quater) {
	if (!quater) {
		for (int i = 0; i < 100; i++) {}// for debug
		return;
	}
	curr_bblock_->addQuater(quater);
}

void IMCode::setCurrBblock(shared_ptr<BasicBlock> bblock) {
	curr_bblock_ = bblock;
}

const vector<shared_ptr<Quaternion>>& IMCode::get_quater_list() {
	if (quater_list_.size() == 0) {
		const auto& gb_list = this->global_bblock_->get_quater_list();
		// global var and const decalre
		quater_list_.insert(quater_list_.end(), gb_list.begin(), gb_list.end());
		// main
		const auto& main_list = this->main_->get_quater_list();
		quater_list_.insert(quater_list_.end(), main_list.begin(), main_list.end());
		// ·Çmainº¯Êý
		for (auto func : this->func_list_) {
			const auto& func_list = func->get_quater_list();
			quater_list_.insert(quater_list_.end(), func_list.begin(), func_list.end());
		}
	}
	return quater_list_;
}

void IMCode::show_quaters(ostream& fout) {
	for (auto quater : this->get_quater_list()) {
		if (quater) {
			fout << quater->toString() << endl;
		} else {
			fout << endl;
		}
	}
}

IMCode::~IMCode() {
}
