#ifndef TYPE_H_
#define TYPE_H_

/*词法分析中的符号类型*/
enum symbolType
{
	IDENFR,				// 标识符
	INTCON,             // 整形常量
	CHARCON,            // 字符常量
	STRCON,             // 字符串
	CONSTTK,            // const
	INTTK,              // int
	CHARTK,             // char
	VOIDTK,             // void
	MAINTK,             // main
	IFTK,               // if
	ELSETK,             // else
	SWITCHTK,           // switch
	CASETK,             // case
	DEFAULTTK,          // default
	WHILETK,            // while
	FORTK,              // for
	SCANFTK,            // scanf
	PRINTFTK,           // printf
	RETURNTK,           // return
	PLUS,               // +
	MINU,              // -
	MULT,               // *
	DIV,                // /
	LSS,                // <
	LEQ,                // <=
	GRE,                // >
	GEQ,                // >=
	EQL,                // ==
	NEQ,                // !=
	COLON,              // :
	ASSIGN,             // =
	SEMICN,             // ;
	COMMA,              // ,
	LPARENT,            // (
	RPARENT,            // )
	LBRACK,             // [
	RBRACK,             // ]
	LBRACE,             // {
	RBRACE,             // }

};

/*符号表中每一项标识符的类型*/
enum EntryType {
	CONST,
	VAR,
	FUNCTION,
	LABEL,
	IMMEDIATE,   // 立即数常量，如123, 'a'
	TEMP,		 // 临时变量，如t1,t2
};

/*符号表中每一项标识符值的类型*/
enum ValueType {
	INTV,
	CHARV,
	VOIDV,
	UNKNOWN, // 出现了未定义的标识符等，导致ValueType无法确定
};


#endif // !TYPE_H_

