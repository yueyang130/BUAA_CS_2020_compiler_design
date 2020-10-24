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
	int row; // 错误所在行
	ErrorType error_type; // 错误类型
	std::string sym;  // 额外的词法信息，帮助我们更快定位错误
	bool operator < (const ErrorInfor& o) { return row < o.row; }

} ErrorInfor;

#endif // !

