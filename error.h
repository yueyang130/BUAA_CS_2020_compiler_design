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
	IllegalAssignmentForConst,
	MissSemi,
	MissRparent,
	MissRbrace,
	IllegalArrayInit,
	UnmatchedConstType,
	MissDefault,
};

typedef struct {
	int row; // ����������
	ErrorType error_type; // ��������
	std::string sym;  // ����Ĵʷ���Ϣ���������Ǹ��춨λ����
	bool operator < (const ErrorInfor& o) { return row < o.row; }

} ErrorInfor;

#endif // !

