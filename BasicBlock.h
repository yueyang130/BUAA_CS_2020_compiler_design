#ifndef BASIC_BLOCK_H_
#define BASIC_BLOCK_H_

#include<vector>
#include<memory>
#include<set>
#include"Quaternion.h"
#include<iostream>


using namespace std;


/*���������Ԫʽ��ʾ
* a.����������еĵ�һ�������������
* b.��ת����ĵ�һ�������������
* c.��ת���ת�Ƶ��ĵ�һ�������������
* d.����������䲻��Ӱ��ֲ�����������һ���᷵�أ���˲�Ӱ�������Ļ���
*
* ÿһ�������䣬ֱ����һ���������������������һ��������
*/
class BasicBlock {
public:
	BasicBlock() { quater_list_.reserve(128);  };
	void addPrevBlock(shared_ptr<BasicBlock> bblock) { pre_set_.insert(bblock);  }
	void addNextBlock(shared_ptr<BasicBlock> bblock) { next_set_.insert(bblock); }
	void addQuater(shared_ptr<Quaternion> quater) { quater_list_.push_back(quater); }
	void active_analysis();
	vector<shared_ptr<Quaternion>>& get_quater_list() { return quater_list_; }
	set<shared_ptr<TableEntry>> active_in_;
	set<shared_ptr<TableEntry>> active_out_;

private:
	set<shared_ptr<BasicBlock>> pre_set_;
	set<shared_ptr<BasicBlock>> next_set_;
	vector<shared_ptr<Quaternion>> quater_list_;
};


#endif // !BASIC_BLOCK_H_




