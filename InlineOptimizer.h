#ifndef INLINE_OPTIMIZER_
#define INLINE_OPTIMIZER_

#include"Optimizer.h"
#include<set>
#include<map>

const int MAX_INLINE_ROW = 100000;

class InlineOptimizer: public Optimizer {
public:
	InlineOptimizer(IMCode imcode);
private:
	void optimize();
	void inline_caller(Function* caller);
	
};   

shared_ptr<TableEntry> do_inline(Function* callee, int param_num, ValueType value_type, vector<shared_ptr<Quaternion>>& quater_list);
bool checkInline(Function* callee);
shared_ptr<Function> findFunction(IMCode& imcode, string func_name);
int containLabel(shared_ptr<Quaternion> quater);
shared_ptr<Quaternion> changeVarQuater(shared_ptr<Quaternion> q0, map<TableEntry*, shared_ptr<TableEntry>>& var_map);
shared_ptr<TableEntry> replaceVar(shared_ptr<TableEntry> entry, map<TableEntry*, shared_ptr<TableEntry>>& var_map);
shared_ptr<Quaternion> changeLabel(shared_ptr<Quaternion> q0, string new_label);

#endif // !INLINE_OPTIMIZER_



