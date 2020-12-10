#include "PeepholeOptimizer.h"

PeepholeOptimizer::PeepholeOptimizer(IMCode imcode):
	Optimizer("peephole_opt", imcode)
{
	this->optmized_imcode_ = this->imcode_;
	// 清空opt_imcode的quater_list,等优化后再重新生成
	this->optmized_imcode_.get_quater_list().clear();

	for (auto func : optmized_imcode_.func_list()) {
		deleteJump(func.get());
		deleteTempVar(func.get());
	}
	deleteJump(&optmized_imcode_.main());
	deleteTempVar(&optmized_imcode_.main());
}

void PeepholeOptimizer::deleteJump(Function* func) {
	auto& quater_list = func->get_quater_list();
	auto it = quater_list.begin(); 
	while (it != quater_list.end()) {
		if ((*it)->quater_type_ == QuaternionType::Goto && (*(it + 1))->quater_type_ == QuaternionType::Label) {
			// goto label1 -> label1:
			if ((*it)->result_->identifier() == (*(it + 1))->result_->identifier()) {
				it = quater_list.erase(it);
			}
			// goro label1 -> label2: -> label1:
			else if ((*(it + 2))->quater_type_ == QuaternionType::Label &&
				(*it)->result_->identifier() == (*(it + 2))->result_->identifier()) {
				it = quater_list.erase(it);
			}
		}
			
		it++;
	}
}

void PeepholeOptimizer::deleteTempVar(Function* func) {
}


