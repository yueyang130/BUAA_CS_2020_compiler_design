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
	FuncReturn,				//		var/temp/c/i/ null
	// ��������
	FuncParamPush,			//								var/const/i
	FuncCall,				//									func
	RetAssign,				//		var
	// �����ͱ�������		
	VarDeclare,				//		var							None/i
	ConstDecalre,			//		const						i
	// ��������ת
	BEQ,					//		label						var/const/i					var/const/i
	BNE,
	BLT,
	BLE,
	BGT,
	BGE,
	Goto,					//		label
	// ���ñ�ǩ
	Label,					//		label
	// ����
	PushArrayIndex,		    //	var/const/i/temp (index)
	GetArrayElem,			//		var							var	(array��			 
	SetArrayELem,			//		var��array)					var/const/i/temp
	// ����
	AddOp,					//		tmp/var						var/const/i/tmp			   var/const/i/tmp
	SubOp,				
	MulOp,
	DivOp,
	Neg,					//		tmp/var						var/const/i/tmp
	// ��ֵ
	Assign,					//		var								temp/var
	// ��д
	Read,					//		var								
	Write,					//									i(�ַ�����/null		  tmp(�������ַ��ͱ��ʽ)/ null			   		
};


/*��Ԫʽ*/
class Quaternion {
public:
	Quaternion(QuaternionType quater_type, shared_ptr<TableEntry> result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right);
	string toString();
	const QuaternionType quater_type_;
	/*ֱ��ʹ�÷��ű�����Ϊ�������ͽ��*/
	const shared_ptr<TableEntry> result_;
	const shared_ptr<TableEntry> opA_;
	const shared_ptr<TableEntry> opB_;
	
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
	static shared_ptr<Quaternion> BEQ(shared_ptr<TableEntry>result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right);
	static shared_ptr<Quaternion> BNE(shared_ptr<TableEntry>result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right);
	static shared_ptr<Quaternion> BLT(shared_ptr<TableEntry>result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right);
	static shared_ptr<Quaternion> BLE(shared_ptr<TableEntry>result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right);
	static shared_ptr<Quaternion> BGT(shared_ptr<TableEntry>result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right);
	static shared_ptr<Quaternion> BGE(shared_ptr<TableEntry>result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right);
	
	static shared_ptr<Quaternion> Goto(shared_ptr<TableEntry> label);
	//static shared_ptr<Quaternion> Bnz(shared_ptr<TableEntry> label);
	//static shared_ptr<Quaternion> Bz(shared_ptr<TableEntry> label);

	static shared_ptr<Quaternion> Label(shared_ptr<TableEntry> label);


	static shared_ptr<Quaternion> pushArrayIndex(shared_ptr<TableEntry>result);
	static shared_ptr<Quaternion> getArrayElem(shared_ptr<TableEntry>result, shared_ptr<TableEntry> left);
	static shared_ptr<Quaternion> setArrayElem(shared_ptr<TableEntry>result, shared_ptr<TableEntry> left);

	static shared_ptr<Quaternion> Add(shared_ptr<TableEntry>result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right);
	static shared_ptr<Quaternion> Sub(shared_ptr<TableEntry>result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right);
	static shared_ptr<Quaternion> Mul(shared_ptr<TableEntry>result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right);
	static shared_ptr<Quaternion> Div(shared_ptr<TableEntry>result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right);
	static shared_ptr<Quaternion> Neg(shared_ptr<TableEntry>result, shared_ptr<TableEntry> left);
	static shared_ptr<Quaternion> Assign(shared_ptr<TableEntry>result, shared_ptr<TableEntry> left);
	static shared_ptr<Quaternion> Read(shared_ptr<TableEntry>result);
	static shared_ptr<Quaternion> Write(shared_ptr<TableEntry>left, shared_ptr<TableEntry> right);
};

/*֧�ֺ���*/
string ValuetypeToString(ValueType value_type);



#endif // ! IM_CODE_H_

