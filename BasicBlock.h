#ifndef BASIC_BLOCK_H_
#define BASIC_BLOCK_H_

#include<vector>
#include<memory>
#include<unordered_set>
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
	BasicBlock();
	void cal_use_and_def();
	void addPrevBlock(shared_ptr<BasicBlock> bblock) { pre_set_.insert(bblock);  }
	void addNextBlock(shared_ptr<BasicBlock> bblock) { next_set_.insert(bblock); }
	void addQuater(shared_ptr<Quaternion> quater) { quater_list_.push_back(quater); }
	/*返回值为true表示进行这一轮活跃变量分析后，in集合没有改变*/
	bool active_analysis();
	vector<shared_ptr<Quaternion>>& get_quater_list() { return quater_list_; }
	// 只对局部变量和临时变量进行活跃变量分析
	unordered_set<shared_ptr<TableEntry>> active_in_;
	unordered_set<shared_ptr<TableEntry>> active_out_;
	void print_bblock(ofstream& fout);

	unordered_set<shared_ptr<TableEntry>> use_;
	unordered_set<shared_ptr<TableEntry>> def_;

private:
	unordered_set<shared_ptr<BasicBlock>> pre_set_;
	unordered_set<shared_ptr<BasicBlock>> next_set_;
	vector<shared_ptr<Quaternion>> quater_list_;

	

	// 如果这个变量不在def_中，把它加入use_
	void add_use_var(shared_ptr<TableEntry> var);
	// 如果这个变量不在use_中，把它加入def_
	void add_def_var(shared_ptr<TableEntry> var);
	
};


#endif // !BASIC_BLOCK_H_




