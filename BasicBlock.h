#ifndef BASIC_BLOCK_H_
#define BASIC_BLOCK_H_

#include<vector>
#include<memory>
#include<unordered_set>
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
	BasicBlock();
	void cal_use_and_def();
	void addPrevBlock(shared_ptr<BasicBlock> bblock) { pre_set_.insert(bblock);  }
	void addNextBlock(shared_ptr<BasicBlock> bblock) { next_set_.insert(bblock); }
	void addQuater(shared_ptr<Quaternion> quater) { quater_list_.push_back(quater); }
	/*����ֵΪtrue��ʾ������һ�ֻ�Ծ����������in����û�иı�*/
	bool active_analysis();
	vector<shared_ptr<Quaternion>>& get_quater_list() { return quater_list_; }
	// ֻ�Ծֲ���������ʱ�������л�Ծ��������
	unordered_set<shared_ptr<TableEntry>> active_in_;
	unordered_set<shared_ptr<TableEntry>> active_out_;
	void print_bblock(ofstream& fout);

	unordered_set<shared_ptr<TableEntry>> use_;
	unordered_set<shared_ptr<TableEntry>> def_;

private:
	unordered_set<shared_ptr<BasicBlock>> pre_set_;
	unordered_set<shared_ptr<BasicBlock>> next_set_;
	vector<shared_ptr<Quaternion>> quater_list_;

	

	// ��������������def_�У���������use_
	void add_use_var(shared_ptr<TableEntry> var);
	// ��������������use_�У���������def_
	void add_def_var(shared_ptr<TableEntry> var);
	
};


#endif // !BASIC_BLOCK_H_




