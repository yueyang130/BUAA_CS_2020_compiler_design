#ifndef TYPE_H_
#define TYPE_H_

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
	MINUS,              // -
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
	END_OF_FILE			// eof

};

#endif // !TYPE_H_

