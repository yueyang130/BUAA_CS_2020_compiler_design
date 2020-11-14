//#ifndef OPT_MIPS_FUNCTION_GENERATOR_
//#define OPT_MIPS_FUNCTION_GENERATOR_
//
//#include"BasicBlock.h"
//#include<map>
//
//using namespace std;
//
///*
//optimized dump:
//Ϊÿһ���ֲ���������ʱ����������һ���ڴ�ռ��ַ��
//�������ֱ���ӳ�䵽ȫ�ּĴ����;ֲ��Ĵ���
//1.ʹ�ñ���ʱ����������ڼĴ�������ֱ��ʹ�üĴ���
//2.ֻ�мĴ�����û�б���ʱ���Ŵ��ڴ���load������
//*/
//
//class MipsFunctionGenerator {
//private:
//	// input and output
//	Function& func_;
//	map<shared_ptr<TableEntry>, int>& global_var_offset_map_;
//	unsigned int* sp;
//	vector<string> mips_code_;
//
//	/*ƫ����*/
//	int offset = 0;
//	/*�ֲ���������ʱ���������sp��ƫ��*/
//	map<shared_ptr<TableEntry>, int> func_var_offset_map_;

//	/* ����ֲ�����������Ϊ�ֲ�������ջ�з����ڴ�ռ��ַ
//	   �����ȫ�ּĴ������У��򽫱���ֵ�������ȫ�ּĴ�����
//	   ����ֵд���ڴ�ռ�
//	   */

//	void map_local_var();
//	/* �������ڵ���ʱ������Ϊ��ʱ������ջ�з����ڴ�ռ��ַ
//	   �������ʱ�Ĵ������У��򽫱���ֵ�������ʱȫ�ּĴ�����
//	   ����ֵд���ڴ�ռ�
//	   */
//	void map_temp_var();
//
//
//public:
//	MipsFunctionGenerator(Function& func, map<shared_ptr<TableEntry>, int>& gb_var_map);
//	vector<string>& getMipsCode() { return mips_code_; }
//
//};
//
//
//#endif // !OPT_MIPS_FUNCTION_GENERATOR_
//
//
