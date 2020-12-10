#include "Optimizer.h"
#include<fstream>

Optimizer::Optimizer(string opt_name, IMCode& imcode):
	opt_name_(opt_name), imcode_(imcode){}

IMCode Optimizer::getOptimizedImcode() {
	return optmized_imcode_;
}

void Optimizer::dump() {
	ofstream target(opt_name_ + ".txt");
	optmized_imcode_.show_quaters(target);
	target.close();
}
