#ifndef PEEPHOLE_OPTIMIZER_
#define PEEPHOLE_OPTIMIZER_

#include"Optimizer.h"

class PeepholeOptimizer: public Optimizer {
public:
	PeepholeOptimizer(IMCode imcode);
private:
	/*无条件跳转指令后面紧跟目标标签，则删除这条无条件跳转*/
	void deleteJump(Function* func);
	/*删除临时变量作为赋值中转站的情况*/
	void deleteTempVar(Function* func);
};


#endif // !PEEPHOLE_OPTIMIZER_



