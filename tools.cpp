#include "tools.h"

/**
* 因为str1和str2是按值传递，所以可以直接在参数上操作
*/
bool strcmp_wo_case(string str1, string str2) {
	transform(str1.begin(), str1.end(), str1.begin(), ::tolower);
	transform(str2.begin(), str2.end(), str2.begin(), ::tolower);
	return str1 == str2;
}
