#ifndef PEEPHOLE_OPTIMIZER_
#define PEEPHOLE_OPTIMIZER_

#include"Optimizer.h"

class PeepholeOptimizer: public Optimizer {
public:
	PeepholeOptimizer(IMCode imcode);
private:
	/*��������תָ��������Ŀ���ǩ����ɾ��������������ת*/
	void deleteJump(Function* func);
	/*ɾ����ʱ������Ϊ��ֵ��תվ�����*/
	void deleteTempVar(Function* func);
};


#endif // !PEEPHOLE_OPTIMIZER_



