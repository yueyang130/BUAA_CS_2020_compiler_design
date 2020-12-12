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
			// goto label1 -> label2: -> label1:
			else if ((*(it + 2))->quater_type_ == QuaternionType::Label &&
				(*it)->result_->identifier() == (*(it + 2))->result_->identifier()) {
				it = quater_list.erase(it);
			}
		}			
		it++;
	}
}

void PeepholeOptimizer::deleteTempVar(Function* func) {
	auto& quater_list = func->get_quater_list();
	auto it = quater_list.begin();
	while (it != quater_list.end()) {
		// 如果it的下一个是label则跳过
		auto it2 = it + 1;
		for (; it2 != quater_list.end() && (*it2)->quater_type_ == QuaternionType::Label; it2++);
		if (is_op_with_assign((*it)->quater_type_) && (*it2)->quater_type_ == QuaternionType::Assign) {
			// 要求必须是临时变量才能删去！如果是局部变量或全局变量是不能删去的！
			if ((*it)->result_.get() == (*it2)->opA_.get() && (*it)->result_->entry_type() == EntryType::TEMP) {
				auto new_quater = make_shared<Quaternion>(**it);
				new_quater->result_ = (*it2)->result_;
				// 先删除后面的，不会影响it的指向
				quater_list.erase(it2);
				it = quater_list.erase(it);
				it = quater_list.insert(it, new_quater);
				it--;
			}
		}
		it++;
	}
}

bool is_op_with_assign(QuaternionType type) {
	return type == QuaternionType::AddOp || type == QuaternionType::SubOp
		|| type == QuaternionType::MulOp || type == QuaternionType::DivOp
		|| type == QuaternionType::Neg
		|| type == QuaternionType::GetArrayElem
		|| type == QuaternionType::Assign;
}


