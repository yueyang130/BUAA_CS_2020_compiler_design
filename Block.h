#ifndef BLOCK_H_
#define BLOCK_H_

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
	BasicBlock() { quater_seq_.reserve(128);  };
	void addPrevSet(shared_ptr<BasicBlock> bblock) { pre_set_.insert(bblock);  }
	void addNextSet(shared_ptr<BasicBlock> bblock) { next_set_.insert(bblock); }
	void addQuater(shared_ptr<Quaternion> quater) { quater_seq_.push_back(quater); }
	void show_quaters(ostream& f);
private:
	set<shared_ptr<BasicBlock>> pre_set_;
	set<shared_ptr<BasicBlock>> next_set_;
	vector<shared_ptr<Quaternion>> quater_seq_;
};

/*�����Ļ������ʾ*/
class Function {
public:
	Function(string& func_name) : func_name_(func_name) {};

	void addBBlock(shared_ptr<BasicBlock>& bblock) { bblock_list_.push_back(bblock); }
	void show_quaters(ostream& f);
private:
	string func_name_;
	/*������ǰ���������л�������б�����Ի��������*/
	/*�б�ĵ�һ��Ԫ��һ���Ǻ��������*/
	vector<shared_ptr<BasicBlock>> bblock_list_;
};

/*����Դ�������Ԫʽ�м����*/
class IMCode {
public:
	static IMCode& getInstance(ostream& f_imcode);

	void addFunc(string& func);
	/*��ǰFunction�����BBlock, ����Ϊcurr_bblock*/
	void addBBlock(shared_ptr<BasicBlock> bblock);
	/*��curr_bblock�������Ԫʽ*/
	void addQuater(shared_ptr<Quaternion> quater) { curr_bblock->addQuater(quater); }
	void show_quaters();

private:
	IMCode(ostream& f_imcode);
	virtual ~IMCode();

	/*����ȫ�ֱ�����ȫ�ֳ����Ļ�����*/
	const shared_ptr<BasicBlock> global_bblock_;
	/*ָ��main������Functionָ��*/
	const shared_ptr<Function> main_;
	/*����Functionָ����б�������main��*/
	vector<shared_ptr<Function>> func_list_;
	/*���������Ԫʽ�Ļ�����*/
	shared_ptr<BasicBlock> curr_bblock;
	/*����ļ���*/
	ostream& f_imcode_;

	void setCurrBblock(shared_ptr<BasicBlock> bblock);


};

#endif // !BLOCK_H_




