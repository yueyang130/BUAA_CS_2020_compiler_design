#ifndef IM_CODER_
#define IM_CODER_

#include<vector>
#include<memory>
#include<set>
#include"Quaternion.h"
#include<iostream>
#include<string>

using namespace std;

/*�����Ļ������ʾ*/
class Function {
public:
	Function(string& func_name) : func_name_(func_name) {};
	void addQuater(shared_ptr<Quaternion> quater) { this->quater_list_.push_back(quater); }
	vector<shared_ptr<Quaternion>>& get_quater_list();
	string name() { return func_name_; }

private:
	string func_name_;
	/*�б�ĵ�һ��Ԫ��һ���Ǻ��������*/
	vector<shared_ptr<Quaternion>> quater_list_;
};


/*����Դ�������Ԫʽ�м����*/
class IMCode {
public:
	static IMCode& getInstance();

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
	const vector<shared_ptr<Quaternion>>& get_quater_list();
	/*�������д�м����*/
	void show_quaters(ostream& fout);


private:
	IMCode();
	virtual ~IMCode();

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

};

#endif // !IM_CODER_



