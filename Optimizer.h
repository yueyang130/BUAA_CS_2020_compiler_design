#ifndef OPTIMIZER_H_
#define OPTIMIZER_H_

#include<vector>
#include<memory>
#include"ImCoder.h"
#include"Quaternion.h"

class Optimizer {
public:
	Optimizer(string opt_name, IMCode& imcode);
	IMCode getOptimizedImcode();
	void dump();

protected:
	string opt_name_;
	IMCode imcode_;
	IMCode optmized_imcode_;

};

#endif // !OPTIMIZER_H_


