#include <iostream>
#include <fstream>
#include <string>
#include "LexicalAnalyzer.h"
#include "GrammerAnalyzer.h"
#include "ImCoder.h"  // ImCoder


// config
#define REG_OPT 0			// 寄存器分配优化和目标代码优化
#define INLINE_OPT 0
#define PEEPHOLE_OPT 0

#if REG_OPT
	#include "OptMipsGenerator.h"
	using namespace OptMips;
#else
// register optimizer
	#include "SimpleMipsGenerator.h"
	using namespace SimpleMips;
#endif

#include "InlineOptimizer.h"
#include "PeepholeOptimizer.h"


using namespace std;

const string INFIFE = "testfile.txt";
const string OUTFILE = "output.txt";
const string ERRFILE = "error.txt";
const string UNOPTFILE = "no_opt_imcode.txt";
const string IRFILE = "IMCode.txt";
const string TARGETFILE = "mips.txt";

int main() {

	ifstream fin(INFIFE);
	//ofstream fout(OUTFILE);
	//ofstream ferror(ERRFILE);
	ofstream f_unopt_code(UNOPTFILE);
	ofstream f_ircode(IRFILE);
	ofstream f_targetcode(TARGETFILE);

	if (!fin.is_open()) {
		cout << "Could not open " << INFIFE << endl;
		exit(EXIT_FAILURE);
	}

	// 词法分析，语法分析，获取未优化的中间代码
	LexicalAnalyzer& myLexicalAnalyzer = LexicalAnalyzer::getInstance(fin);
	IMCode myIMCode = IMCode();
	GrammerAnalyzer& myGrammerAnalyzer = GrammerAnalyzer::getInstance(myLexicalAnalyzer, myIMCode);
	myLexicalAnalyzer.analyzeLexis();
	//myLexicalAnalyzer.show();
	myGrammerAnalyzer.analyzeGrammer();
	//myGrammerAnalyzer.show(fout);
	//myGrammerAnalyzer.showError(ferror);
	myIMCode.show_quaters(f_unopt_code);


	// 中间代码的内联优化
	if (INLINE_OPT) {
		InlineOptimizer inline_opt(myIMCode);
		myIMCode = inline_opt.getOptimizedImcode();
		inline_opt.dump();
	}
	// 窥孔优化
	if (PEEPHOLE_OPT) {
		PeepholeOptimizer peephole_opt(myIMCode);
		myIMCode = peephole_opt.getOptimizedImcode();
		peephole_opt.dump();
	}

	// 划分基本块和活跃变量计算
	myIMCode.divide_bblock();
	myIMCode.active_analysis();
	// 输出基本块划分和活跃变量分析的结果
	myIMCode.print_bblock();
	// 输出中间代码
	myIMCode.show_quaters(f_ircode);

	// 生成目标代码
	MipsGenerator& myMipsGenerator = MipsGenerator::getInstance(myIMCode);
	myMipsGenerator.generateMipsCode();
	myMipsGenerator.showMipsCode(f_targetcode);


	// 关闭优化后的输出文件流
	fin.close();
	//fout.close();
	//ferror.close();
	f_unopt_code.close();
	f_ircode.close();
	f_targetcode.close();


	return 0;
}