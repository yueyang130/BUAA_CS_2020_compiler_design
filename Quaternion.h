#ifndef IM_CODE_H_
#define IM_CODE_H_

#include<string>
#include<memory>
#include"SymbolTable.h"

using namespace std;

/*i : ������*/
/*		��Ԫʽ����					result						left						right	*/
enum QuaternionType {
	// ��������
	FuncDeclareHead,		//		func
	FuncFormalParam,		//		var
	FuncReturn,				//									var
	// ��������
	FuncParamPush,			//								var/const/i
	FuncCall,				//									func
	RetAssign,				//		var
	// �����ͱ�������		
	VarDeclare,				//		var							None/i
	ConstDecalre,			//		const						i
	// ���ʽ
	Expr,					//		var							var/const/i					var/const/i
	// ��������ת
	Condition,				//									var/const/i					var/const/i
	Goto,					//									label
	Bnz,					//									label
	Bz,						//									label
	// ���ñ�ǩ
	Label,					//		label
	// ����
	GetArrayElem,			//		var							var	(array��			   var/const/i (index)
	SetArrayELem,			//		var��array)					var/const/i(index)		   var/const/i
	// ����
	AddOp,					//		tmp/var						var/const/i/tmp      var/const/i/tmp
	SubOp,				
	MulOp,
	DivOp,
	Neg,					//		tmp/var						var/const/i/tmp
};


/*��Ԫʽ*/
class Quaternion {
public:
	Quaternion(QuaternionType quater_type, shared_ptr<TableEntry> result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right);
	string toString();

private:
	QuaternionType quater_type_;
	/*ֱ��ʹ�÷��ű�����Ϊ�������ͽ��*/
	shared_ptr<TableEntry> result_;
	shared_ptr<TableEntry> left_;
	shared_ptr<TableEntry> right_;
};


/*�򵥹���ģʽ������Ԫʽ*/
class QuaternionFactory {
public:
	static shared_ptr<Quaternion> FuncDeclareHead(shared_ptr<TableEntry> func);
	static shared_ptr<Quaternion> FuncFormalParam(shared_ptr<TableEntry> var);
	static shared_ptr<Quaternion> FuncReturn(shared_ptr<TableEntry> var);

	static shared_ptr<Quaternion> FuncParamPush(shared_ptr<TableEntry> var);
	static shared_ptr<Quaternion> FuncCall(shared_ptr<TableEntry> func);
	static shared_ptr<Quaternion> RetAssign(shared_ptr<TableEntry> var);
	
	static shared_ptr<Quaternion> VarDecalre(shared_ptr<TableEntry> var, shared_ptr<TableEntry> immediate = nullptr);
	static shared_ptr<Quaternion> ConstDeclare(shared_ptr<TableEntry> con);
	static shared_ptr<Quaternion> Expr(shared_ptr<TableEntry>result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right);
	static shared_ptr<Quaternion> Condition(shared_ptr<TableEntry> left, shared_ptr<TableEntry> right);
	
	static shared_ptr<Quaternion> Goto(shared_ptr<TableEntry> label);
	static shared_ptr<Quaternion> Bnz(shared_ptr<TableEntry> label);
	static shared_ptr<Quaternion> Bz(shared_ptr<TableEntry> label);

	static shared_ptr<Quaternion> Label(shared_ptr<TableEntry> label);

	static shared_ptr<Quaternion> getArrayElem(shared_ptr<TableEntry>result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right);
	static shared_ptr<Quaternion> setArrayElem(shared_ptr<TableEntry>result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right);

	static shared_ptr<Quaternion> Add(shared_ptr<TableEntry>result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right);
	static shared_ptr<Quaternion> Sub(shared_ptr<TableEntry>result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right);
	static shared_ptr<Quaternion> Mul(shared_ptr<TableEntry>result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right);
	static shared_ptr<Quaternion> Div(shared_ptr<TableEntry>result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right);
	static shared_ptr<Quaternion> Neg(shared_ptr<TableEntry>result, shared_ptr<TableEntry> left);
};

/*֧�ֺ���*/
inline string ValuetypeToString(ValueType value_type) {
	switch (value_type) {
	case ValueType::CHARV:
		return "char";
	case ValueType::INTV:
		return "int";
	case ValueType::VOIDV:
		return "void";
	default:
		return "unknown";
	}
}





#endif // ! IM_CODE_H_

