#ifndef IM_CODE_H_
#define IM_CODE_H_

#include<string>
#include<memory>
#include"SymbolTable.h"

using namespace std;

/*i : 立即数*/
/*		四元式类型					result						left						right	*/
enum QuaternionType {
	// 函数声明
	FuncDeclareHead,		//		func
	FuncFormalParam,		//		var
	FuncReturn,				//									var
	// 函数调用
	FuncParamPush,			//								var/const/i
	FuncCall,				//									func
	RetAssign,				//		var
	// 常量和变量声明		
	VarDeclare,				//		var							None/i
	ConstDecalre,			//		const						i
	// 表达式
	Expr,					//		var							var/const/i					var/const/i
	// 条件和跳转
	Condition,				//									var/const/i					var/const/i
	Goto,					//									label
	Bnz,					//									label
	Bz,						//									label
	// 设置标签
	Label,					//		label
	// 数组
	GetArrayElem,			//		var							var	(array）			   var/const/i (index)
	SetArrayELem,			//		var（array)					var/const/i(index)		   var/const/i
	// 算术
	AddOp,					//		tmp/var						var/const/i/tmp      var/const/i/tmp
	SubOp,				
	MulOp,
	DivOp,
	Neg,					//		tmp/var						var/const/i/tmp
};


/*四元式*/
class Quaternion {
public:
	Quaternion(QuaternionType quater_type, shared_ptr<TableEntry> result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right);
	string toString();

private:
	QuaternionType quater_type_;
	/*直接使用符号表项作为操作数和结果*/
	shared_ptr<TableEntry> result_;
	shared_ptr<TableEntry> left_;
	shared_ptr<TableEntry> right_;
};


/*简单工厂模式生成四元式*/
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

/*支持函数*/
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

