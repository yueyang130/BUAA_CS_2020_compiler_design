#include "tools.h"

/**
* ��Ϊstr1��str2�ǰ�ֵ���ݣ����Կ���ֱ���ڲ����ϲ���
*/
bool strcmp_wo_case(string str1, string str2) {
	transform(str1.begin(), str1.end(), str1.begin(), ::tolower);
	transform(str2.begin(), str2.end(), str2.begin(), ::tolower);
	return str1 == str2;
}
