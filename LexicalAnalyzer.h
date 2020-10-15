#ifndef LEXICAL_ANALYZER_H_
#define LEXICAL_ANALYZER_H_

#include <map>
#include <fstream>
#include <string>
#include <vector>
#include "type.h"

using namespace std;

const int INIT_LENGTH = 4096;                                   // 保存单词，单词类别，行数等的列表的初始长度																														 
																														
typedef struct {
	string sym;   // 存储单词的字符串列表   
	symbolType type;   // 存储单词类别的列表     
	int row;   // 存储单词所在行的列表
				// 可以由type和sym推断出单词值
} SymInfor;



class LexicalAnalyzer
{

	friend class GrammerAnalyzer;
public:

	static LexicalAnalyzer& getInstance(istream& fin);         // single-instance pattern
	void analyzeLexis();                                             // 进行词法分析的核心函数
	void show(ostream& fout);                                  // 将词法分析的结果输出到文件中

private:
	istream& fin;										        // 输入文件流

	char chrCurr;                                               // 当前读取的字符
	string symCurr;                                             // 当前读取的符号
	symbolType  symType;                                        // 当前读取的符号的类别
	bool needRetract;								            // 当要回退一个字符时，将needRetract设为1
													   
	vector<SymInfor> sym_infor_list_;							// 存储词法分析结果的列表


	int rowCnt;                                                 // 当前读取的行数
	int colCnt;                                                 // 当前读取的列数

	map<string, symbolType> reserverMap;                        // 将保留字的字符串映射到单词类别的map


	LexicalAnalyzer(istream& fin);                             // 构造函数
	void nextChar();				                        // 获取下一个字符
	void nextSym();                                             // 获取下一个symbol
	bool isReserver();						                    // sym是否是保留字; 如果是，将类型存到symType			 
	string type_to_str(symbolType);								
											
	/*支持函数*/								                    // 定义内联函数，简化代码
	inline void retract() { needRetract = true; }				// 回退一个字符
	inline void catToken() { symCurr.append(1, chrCurr); }		
	inline void addInfo() {
		SymInfor new_sym_infor = { symCurr, symType, rowCnt };
		sym_infor_list_.push_back(new_sym_infor);
	}
	inline bool isLetter() {									// '_'也算是字母
		return isalpha(chrCurr) || chrCurr == '_';
	}   
	inline bool isChr() {			                            // ＜字符＞    ::=  '＜加法运算符＞'｜'＜乘法运算符＞'｜'＜字母＞'｜'＜数字＞'   
		return chrCurr == '+' || chrCurr == '-' || chrCurr == '*'
			|| chrCurr == '/' || isLetter() || isdigit(chrCurr);
	}
	inline bool isInStr() {                                     // ＜字符串＞   ::=  "｛十进制编码为32,33,35-126的ASCII字符｝
		return chrCurr == 32 || chrCurr == 33 || (chrCurr >= 35 && chrCurr <= 126);
	}
	inline void error() { ; }
};


#endif                                                          // !LEXICAL_ANALYZER_

