#ifndef ERROR_H_
#define ERROR_H_

#include <string>

enum ErrorType {
	IllegalLexis,
	Redefine,
	Undefine,
	FuncParamNumUnmatch,
	FuncParamTypeUnmatch,
	IllegalCondition,
	ReturnErrorInVoidFunc,
	ReturnErrorInNonvoidFunc,
	IllegalArrayIndexType,
	IllegalAssignmentToConst,
	MissSemi,
	MissRparent,
	MissRbrack,
	IllegalArrayInit,
	UnmatchedConstType,
	MissDefault,
};

struct ErrorInfor{
	int row; // ����������
	ErrorType error_type; // ��������
	std::string sym;  // ����Ĵʷ���Ϣ���������Ǹ��춨λ����
	bool operator<(ErrorInfor& o) { return row < o.row; }

} ;



#endif // !

