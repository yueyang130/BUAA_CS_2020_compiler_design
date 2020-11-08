#ifndef TYPE_H_
#define TYPE_H_

/*�ʷ������еķ�������*/
enum symbolType
{
	IDENFR,				// ��ʶ��
	INTCON,             // ���γ���
	CHARCON,            // �ַ�����
	STRCON,             // �ַ���
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

/*���ű���ÿһ���ʶ��������*/
enum EntryType {
	CONST,
	VAR,
	FUNCTION,
	LABEL,
	IMMEDIATE,   // ��������������123, 'a'
	TEMP,		 // ��ʱ��������t1,t2
};

/*���ű���ÿһ���ʶ��ֵ������*/
enum ValueType {
	INTV,
	CHARV,
	VOIDV,
	UNKNOWN, // ������δ����ı�ʶ���ȣ�����ValueType�޷�ȷ��
};


#endif // !TYPE_H_

