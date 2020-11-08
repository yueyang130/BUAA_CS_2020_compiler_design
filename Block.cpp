#include "Block.h"

void BasicBlock::show_quaters(ostream& f) {
	for (auto quater : this->quater_seq_) {
		f << quater->toString();
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
	func_list_.push_back(make_shared<Function>(func));
}

void IMCode::addBBlock(shared_ptr<BasicBlock> bblock) {
	func_list_.back()->addBBlock(bblock);
	curr_bblock = bblock;
}

void IMCode::setCurrBblock(shared_ptr<BasicBlock> bblock) {
	curr_bblock = bblock;
}

void IMCode::show_quaters() {
	global_bblock_->show_quaters(f_imcode_);
	main_->show_quaters(f_imcode_);
	for (auto func : this->func_list_) {
		func->show_quaters(f_imcode_);
	}
}

IMCode::IMCode(ostream& f_imcode): 
	f_imcode_(f_imcode), global_bblock_(new BasicBlock()), main_(new Function(string("main"))){
	curr_bblock = global_bblock_;
}

IMCode::~IMCode() {
}

