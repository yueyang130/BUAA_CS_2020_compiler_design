#ifndef LEXICAL_ANALYZER_H_
#define LEXICAL_ANALYZER_H_

#include <map>
#include <fstream>
#include <string>
#include <vector>
#include "type.h"
#include "error.h"

using namespace std;

const int INIT_LENGTH = 4096;                                   // ���浥�ʣ�������������ȵ��б�ĳ�ʼ����																														 

struct SymInfor{
	string sym;   // �洢���ʵ��ַ����б�   
	symbolType type;   // �洢���������б�     
	int row;   // �洢���������е��б�
				// ������type��sym�ƶϳ�����ֵ
};


class LexicalAnalyzer
{

	friend class GrammerAnalyzer;
public:

	static LexicalAnalyzer& getInstance(istream& fin);         // single-instance pattern
	void analyzeLexis();                                             // ���дʷ������ĺ��ĺ���
	void show(ostream& fout);                                  // ���ʷ������Ľ��������ļ���

private:
	istream& fin;										        // �����ļ���

	char chrCurr;                                               // ��ǰ��ȡ���ַ�
	string symCurr;                                             // ��ǰ��ȡ�ķ���
	symbolType  symType;                                        // ��ǰ��ȡ�ķ��ŵ����
	bool needRetract;								            // ��Ҫ����һ���ַ�ʱ����needRetract��Ϊ1
													   
	vector<SymInfor> sym_infor_list_;							// �洢�ʷ�����������б�
	vector<ErrorInfor> error_infor_list_;						// ������Ϣ�洢�б�

	int rowCnt;                                                 // ��ǰ��ȡ������
	int colCnt;                                                 // ��ǰ��ȡ������

	map<string, symbolType> reserverMap;                        // �������ֵ��ַ���ӳ�䵽��������map


	LexicalAnalyzer(istream& fin);                             // ���캯��
	void nextChar();				                        // ��ȡ��һ���ַ�
	void nextSym();                                             // ��ȡ��һ��symbol
	bool isReserver();						                    // sym�Ƿ��Ǳ�����; ����ǣ������ʹ浽symType			 
	string type_to_str(symbolType);								
											
	/*֧�ֺ���*/								                    // ���������������򻯴���
	inline void retract() { needRetract = true; }				// ����һ���ַ�
	inline void catToken() { symCurr.append(1, chrCurr); }		// ����ǰchar����sym
	inline void addInfo() {
		SymInfor new_sym_infor = { symCurr, symType, rowCnt };
		sym_infor_list_.push_back(new_sym_infor);
	}
	inline bool isLetter() {									// '_'Ҳ������ĸ
		return isalpha(chrCurr) || chrCurr == '_';
	}   
	inline bool isChr() {			                            // ���ַ���    ::=  '���ӷ��������'��'���˷��������'��'����ĸ��'��'�����֣�'   
		return chrCurr == '+' || chrCurr == '-' || chrCurr == '*'
			|| chrCurr == '/' || isLetter() || isdigit(chrCurr);
	}
	inline bool isInStr() {                                     // ���ַ�����   ::=  "��ʮ���Ʊ���Ϊ32,33,35-126��ASCII�ַ���
		return chrCurr == 32 || chrCurr == 33 || (chrCurr >= 35 && chrCurr <= 126);
	}
	/**
	*����������
	* 1. ��������Ϣ����error_infor_list_
	* 2. ����ǰ������ַ�����sym
	*/
	inline void error(string extra_infor = "") {
		ErrorInfor new_error_infor = { rowCnt, ErrorType::IllegalLexis, extra_infor};
		error_infor_list_.push_back(new_error_infor);
		catToken();
	}
};


#endif                                                          // !LEXICAL_ANALYZER_

