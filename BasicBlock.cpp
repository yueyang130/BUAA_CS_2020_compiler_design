#include "BasicBlock.h"
#include"tools.h"

void BasicBlock::show_quaters(ostream& f) {
	for (auto quater : this->quater_seq_) {
		f << quater->toString() << endl;
	}
}


void Function::show_quaters(ostream& f) {
	for (auto bblock : this->bblock_list_) {
		bblock->show_quaters(f);
	}
}


IMCode& IMCode::getInstance(ostream& f_imcode) {
	static IMCode im_cooder(f_imcode);
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

void IMCode::setCurrBblock(shared_ptr<BasicBlock> bblock) {
	curr_bblock_ = bblock;
}

void IMCode::show_quaters() {
	global_bblock_->show_quaters(f_imcode_);
	main_->show_quaters(f_imcode_);
	for (auto func : this->func_list_) {
		func->show_quaters(f_imcode_);
	}
}

IMCode::IMCode(ostream& f_imcode): 
	global_bblock_(new BasicBlock()), f_imcode_(f_imcode) {
	curr_bblock_ = global_bblock_;
}

IMCode::~IMCode() {
}

