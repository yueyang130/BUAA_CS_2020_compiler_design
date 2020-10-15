#ifndef LEXICAL_ANALYZER_H_
#define LEXICAL_ANALYZER_H_

#include <map>
#include <fstream>
#include <string>
#include <vector>
#include "type.h"

using namespace std;

const int INIT_LENGTH = 4096;                                   // ���浥�ʣ�������������ȵ��б�ĳ�ʼ����																														 
																														
typedef struct {
	string sym;   // �洢���ʵ��ַ����б�   
	symbolType type;   // �洢���������б�     
	int row;   // �洢���������е��б�
				// ������type��sym�ƶϳ�����ֵ
} SymInfor;



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
	inline void catToken() { symCurr.append(1, chrCurr); }		
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
	inline void error() { ; }
};


#endif                                                          // !LEXICAL_ANALYZER_

