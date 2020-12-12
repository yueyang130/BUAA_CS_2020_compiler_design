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

/*函数的基本块表示*/
class Function {
public:
	Function(string& func_name) : func_name_(func_name) {};
	void addQuater(shared_ptr<Quaternion> quater) { this->quater_list_.push_back(quater); }
	vector<shared_ptr<Quaternion>>& get_quater_list();
	string name() { return func_name_; }
	/*查找quater所在的基本块*/
	BasicBlock* getBBlock(shared_ptr<Quaternion> quater);
	/*划分基本块*/
	void divide_bblock();
	/*活跃变量分析*/
	void active_analysis();

private:
	string func_name_;
	/*列表的第一个元素一定是函数的入口*/
	vector<shared_ptr<Quaternion>> quater_list_;
	/*所有四元式到基本块的映射，用于查找四元式属于哪一个基本块*/
	map<Quaternion*, BasicBlock*> quater_bblock_map_;
	/*基本块列表*/
	vector<BasicBlock*> bblock_list_;
};


/**
* 1. 存储全局声明的四元式和各个Function的四元式
* 2. 含有整个源代码的四元式中间代码Vector
* 3. 负责基本块的划分，基本块的划分放在部分中间代码优化之后
*/
class IMCode {
public:
	IMCode();
	virtual ~IMCode();

	/*添加函数定义，会自动区分main函数和其他函数; 将添加的函数设置为curr_func*/
	void addFunc(string& func);
	/*向curr_func中添加四元式*/
	void addQuater(shared_ptr<Quaternion> quater);
	/*获取curr_func的引用*/
	Function* curr_func() { return curr_func_.get(); }
	vector<shared_ptr<Quaternion>>& global_list() { return gb_list_;  }
	Function& main() { return *main_; }
	/*获取整个源代码的中间代码*/
	vector<shared_ptr<Function>>& func_list() { return func_list_; }
	/*在整个文件的中间代码全部生成完毕后，才能调用此函数返回一个完整的列表*/
	vector<shared_ptr<Quaternion>>& get_quater_list();
	/*向输出流写中间代码*/
	void show_quaters(ostream& fout);
	/*划分基本块*/
	void divide_bblock();
	/*活跃变量分析*/
	void active_analysis();


private:
	/*声明全局变量和全局常量的基本块*/
	//shared_ptr<BasicBlock> global_bblock_;
	vector<shared_ptr<Quaternion>> gb_list_;
	/*指向main函数的Function指针*/
	shared_ptr<Function> main_;
	/*所有Function指针的列表（不包含main）*/
	vector<shared_ptr<Function>> func_list_;
	/*正在生成中间代码的函数*/
	shared_ptr<Function> curr_func_ = nullptr;
	/*整个文件的中间代码*/
	vector<shared_ptr<Quaternion>> quater_list_;
	/*基本块*/
	vector<shared_ptr<BasicBlock>> bblock_list_;

};

bool is_con_jump(QuaternionType type);

bool is_uncon_jump(QuaternionType type);


#endif // !IM_CODER_

