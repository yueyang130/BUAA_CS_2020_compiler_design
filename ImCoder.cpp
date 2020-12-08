#include "ImCoder.h"
#include"tools.h"


vector<shared_ptr<Quaternion>>& Function::get_quater_list() {
	
	return quater_list_;
}

IMCode::IMCode() {
}

IMCode::~IMCode() {
}

IMCode& IMCode::getInstance() {
	static IMCode im_coder;
	return im_coder;
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


const vector<shared_ptr<Quaternion>>& IMCode::get_quater_list() {
	if (quater_list_.size() == 0) {
		// global var and const decalre
		quater_list_.insert(quater_list_.end(), gb_list_.begin(), gb_list_.end());
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



