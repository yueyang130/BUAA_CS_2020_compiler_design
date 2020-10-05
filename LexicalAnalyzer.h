#ifndef LEXICAL_ANALYZER_H_
#define LEXICAL_ANALYZER_H_

#include <map>
#include <fstream>
#include <string>
#include <vector>
#include "type.h"

using namespace std;

const int INIT_LENGTH = 4096;                          // ���浥�ʣ�������������ȵ��б�ĳ�ʼ����


class LexicalAnalyzer
{
public:

	static LexicalAnalyzer& getInstance(ifstream& fin); // single-instance pattern
	void analyze();                                    // ���дʷ������ĺ��ĺ���
	void show(ofstream& fout);                          // ���ʷ������Ľ��������ļ���

private:
	ifstream& fin;										// �����ļ���

	char chrCurr;                                     // ��ǰ��ȡ���ַ�
	string symCurr;                                    // ��ǰ��ȡ�ķ���
	symbolType  symType;                               // ��ǰ��ȡ�ķ��ŵ����
	bool needRetract;								   // ��Ҫ����һ���ַ�ʱ����needRetract��Ϊ1
													   
													   // �洢�ʷ�����������б�
	vector<string> symList;                            // �洢���ʵ��ַ����б�
	vector<symbolType> symTypeList;                    // �洢���������б�
	vector<int> symRowList;                            // �洢���������е��б�
													   // ������type��sym�ƶϳ�����ֵ
	int rowCnt;                                        // ��ǰ��ȡ������
	int colCnt;                                        // ��ǰ��ȡ������

	map<string, symbolType> reserverMap;                  // �������ֵ��ַ���ӳ�䵽��������map


	LexicalAnalyzer(ifstream& fin);                     // ���캯��
	void nextChar();				                   // ��ȡ��һ���ַ�
	void nextSym();                                    // ��ȡ��һ��symbol
	bool isReserver();						// sym�Ƿ��Ǳ�����; ����ǣ������ʹ浽symType			 
	
											
											// ���������������򻯴���
	inline void retract() { needRetract = true; }				// ����һ���ַ�
	inline void catToken() { symCurr.append(1, chrCurr); }		
	inline void addInfo() {
		symList.push_back(symCurr);
		symTypeList.push_back(symType);
		symRowList.push_back(rowCnt);
	}
	inline bool isChr() {			// ���ַ���    ::=  '���ӷ��������'��'���˷��������'��'����ĸ��'��'�����֣�'   
		return chrCurr == '+' || chrCurr == '-' || chrCurr == '*'
			|| chrCurr == '/' || isalpha(chrCurr) || isdigit(chrCurr);
	}
	inline bool isInStr() {  // ���ַ�����   ::=  "��ʮ���Ʊ���Ϊ32,33,35-126��ASCII�ַ���
		return chrCurr == 32 || chrCurr == 33 || (chrCurr >= 35 && chrCurr <= 126);
	}
};


#endif // !LEXICAL_ANALYZER_

