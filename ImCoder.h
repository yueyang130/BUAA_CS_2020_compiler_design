#ifndef IM_CODER_
#define IM_CODER_

#include<vector>
#include<memory>
#include<set>
#include"Quaternion.h"
#include<iostream>
#include<string>
#include"BasicBlock.h"
#include<map>

using namespace std;

/*�����Ļ������ʾ*/
class Function {
public:
	Function(string& func_name) : func_name_(func_name) {};
	void addQuater(shared_ptr<Quaternion> quater) { this->quater_list_.push_back(quater); }
	vector<shared_ptr<Quaternion>>& get_quater_list();
	string name() { return func_name_; }
	/*����quater���ڵĻ�����*/
	BasicBlock* getBBlock(shared_ptr<Quaternion> quater);
	/*���ֻ�����*/
	void divide_bblock();
	/*��Ծ��������*/
	void active_analysis();

private:
	string func_name_;
	/*�б�ĵ�һ��Ԫ��һ���Ǻ��������*/
	vector<shared_ptr<Quaternion>> quater_list_;
	/*������Ԫʽ���������ӳ�䣬���ڲ�����Ԫʽ������һ��������*/
	map<Quaternion*, BasicBlock*> quater_bblock_map_;
	/*�������б�*/
	vector<BasicBlock*> bblock_list_;
};


/**
* 1. �洢ȫ����������Ԫʽ�͸���Function����Ԫʽ
* 2. ��������Դ�������Ԫʽ�м����Vector
* 3. ���������Ļ��֣�������Ļ��ַ��ڲ����м�����Ż�֮��
*/
class IMCode {
public:
	IMCode();
	virtual ~IMCode();

	/*��Ӻ������壬���Զ�����main��������������; ����ӵĺ�������Ϊcurr_func*/
	void addFunc(string& func);
	/*��curr_func�������Ԫʽ*/
	void addQuater(shared_ptr<Quaternion> quater);
	/*��ȡcurr_func������*/
	Function* curr_func() { return curr_func_.get(); }
	vector<shared_ptr<Quaternion>>& global_list() { return gb_list_;  }
	Function& main() { return *main_; }
	/*��ȡ����Դ������м����*/
	vector<shared_ptr<Function>>& func_list() { return func_list_; }
	/*�������ļ����м����ȫ��������Ϻ󣬲��ܵ��ô˺�������һ���������б�*/
	vector<shared_ptr<Quaternion>>& get_quater_list();
	/*�������д�м����*/
	void show_quaters(ostream& fout);
	/*���ֻ�����*/
	void divide_bblock();
	/*��Ծ��������*/
	void active_analysis();


private:
	/*����ȫ�ֱ�����ȫ�ֳ����Ļ�����*/
	//shared_ptr<BasicBlock> global_bblock_;
	vector<shared_ptr<Quaternion>> gb_list_;
	/*ָ��main������Functionָ��*/
	shared_ptr<Function> main_;
	/*����Functionָ����б�������main��*/
	vector<shared_ptr<Function>> func_list_;
	/*���������м����ĺ���*/
	shared_ptr<Function> curr_func_ = nullptr;
	/*�����ļ����м����*/
	vector<shared_ptr<Quaternion>> quater_list_;
	/*������*/
	vector<shared_ptr<BasicBlock>> bblock_list_;

};

bool is_con_jump(QuaternionType type);

bool is_uncon_jump(QuaternionType type);


#endif // !IM_CODER_

