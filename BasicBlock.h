#ifndef BASIC_BLOCK_H_
#define BASIC_BLOCK_H_

#include<vector>
#include<memory>
#include<set>
#include"Quaternion.h"
#include<iostream>

using namespace std;


/*基本块的四元式表示
* a.整个语句序列的第一个语句是入口语句
* b.跳转语句后的第一个语句是入口语句
* c.跳转语句转移到的第一个语句是入口语句
* d.函数调用语句不会影响局部变量，而且一定会返回，因此不影响基本块的划分
*
* 每一个入口语句，直到下一个入口语句或程序结束，构成一个基本块
*/
class BasicBlock {
public:
	BasicBlock() { quater_list_.reserve(128);  };
	void addPrevSet(shared_ptr<BasicBlock> bblock) { pre_set_.insert(bblock);  }
	void addNextSet(shared_ptr<BasicBlock> bblock) { next_set_.insert(bblock); }
	void addQuater(shared_ptr<Quaternion> quater) { quater_list_.push_back(quater); }
	const vector<shared_ptr<Quaternion>>& get_quater_list();

private:
	set<shared_ptr<BasicBlock>> pre_set_;
	set<shared_ptr<BasicBlock>> next_set_;
	vector<shared_ptr<Quaternion>> quater_list_;
};


/*函数的基本块表示*/
class Function {
public:
	Function(string& func_name) : func_name_(func_name) {};

	void addBBlock(shared_ptr<BasicBlock>& bblock) { bblock_list_.push_back(bblock); }
	const vector<shared_ptr<Quaternion>>& get_quater_list();

private:
	string func_name_;
	/*包含当前函数内所有基本块的列表，方便对基本块遍历*/
	/*列表的第一个元素一定是函数的入口*/
	vector<shared_ptr<BasicBlock>> bblock_list_;
	vector<shared_ptr<Quaternion>> quater_list_;
};


/*整个源代码的四元式中间代码*/
class IMCode {
public:
	static IMCode& getInstance();

	/*添加函数定义，会自动区分main函数和其他函数*/
	void addFunc(string& func);
	/*向当前Function中添加BBlock, 并设为curr_bblock*/
	void addBBlock(shared_ptr<BasicBlock> bblock);
	/*向curr_bblock中添加四元式*/
	void addQuater(shared_ptr<Quaternion> quater) { curr_bblock_->addQuater(quater); }
	shared_ptr<BasicBlock> curr_bblock() { return curr_bblock_; }
	const vector<shared_ptr<Quaternion>>& get_quater_list();
	void show_quaters(ostream& fout);
	BasicBlock& global_bblock() { return *global_bblock_;  }

private:
	IMCode();
	virtual ~IMCode();

	/*声明全局变量和全局常量的基本块*/
	shared_ptr<BasicBlock> global_bblock_;
	/*指向main函数的Function指针*/
	shared_ptr<Function> main_;
	/*所有Function指针的列表（不包含main）*/
	vector<shared_ptr<Function>> func_list_;
	/*正在添加四元式的基本块*/
	shared_ptr<BasicBlock> curr_bblock_;
	/*正在添加基本块的函数*/
	shared_ptr<Function> curr_func_;
	/*整个文件的中间代码*/
	vector<shared_ptr<Quaternion>> quater_list_;

	void setCurrBblock(shared_ptr<BasicBlock> bblock);


};

#endif // !BASIC_BLOCK_H_




