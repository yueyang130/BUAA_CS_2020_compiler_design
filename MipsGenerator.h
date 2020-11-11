#ifndef MIPS_GENERATOR_H_
#define MIPS_GENERATOR_H_

#include"BasicBlock.h"
#include"Quaternion.h"
#include<iostream>

using namespace std;

class MipsGenerator {
public:
	MipsGenerator(IMCode& im_code, ostream& fout);
	void generateMipsCode();
	void showMipsCode();
private:
	IMCode& im_code_;
	ostream& fout_;
};

/*将四元式转化为MIPS的函数*/


#endif // !MIPS_GENERATOR_H_



