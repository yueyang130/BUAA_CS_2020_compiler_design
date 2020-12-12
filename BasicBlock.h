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
	void addPrevBlock(shared_ptr<BasicBlock> bblock) { pre_set_.insert(bblock.get());  }
	void addNextBlock(shared_ptr<BasicBlock> bblock) { next_set_.insert(bblock.get()); }
	void addQuater(shared_ptr<Quaternion> quater) { quater_list_.push_back(quater); }
	void active_analysis();
	vector<shared_ptr<Quaternion>>& get_quater_list() { return quater_list_; }
	set<TableEntry*> active_in_;
	set<TableEntry*> active_out_;

private:
	set<BasicBlock*> pre_set_;
	set<BasicBlock*> next_set_;
	vector<shared_ptr<Quaternion>> quater_list_;
};


#endif // !BASIC_BLOCK_H_




