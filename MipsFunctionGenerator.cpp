#include "MipsFunctionGenerator.h"


MipsFunctionGenerator::MipsFunctionGenerator(Function& func, map<shared_ptr<TableEntry>, int>& gb_var_map) :
	func_(func), global_var_offset_map_(gb_var_map)
{
	
}

void MipsFunctionGenerator::generateMipsCode() {
	int framesize = 0; // ͳ�ƺ���ջ֡��С
	for (auto& quater : func_.get_quater_list()) {

	}
 
}

