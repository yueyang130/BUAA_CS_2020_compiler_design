#include "PeepholeOptimizer.h"

PeepholeOptimizer::PeepholeOptimizer(IMCode imcode):
	Optimizer("peephole_opt", imcode)
{
	this->optmized_imcode_ = this->imcode_;
	// ���opt_imcode��quater_list,���Ż�������������
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
		// ���it����һ����label������
		auto it2 = it + 1;
		for (; it2 != quater_list.end() && (*it2)->quater_type_ == QuaternionType::Label; it2++);
		if (is_op_with_assign((*it)->quater_type_) && (*it2)->quater_type_ == QuaternionType::Assign) {
			// Ҫ���������ʱ��������ɾȥ������Ǿֲ�������ȫ�ֱ����ǲ���ɾȥ�ģ�
			if ((*it)->result_.get() == (*it2)->opA_.get() && (*it)->result_->entry_type() == EntryType::TEMP) {
				auto new_quater = make_shared<Quaternion>(**it);
				new_quater->result_ = (*it2)->result_;
				// ��ɾ������ģ�����Ӱ��it��ָ��
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


