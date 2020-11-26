#include"Quaternion.h"
#include<sstream>

Quaternion::Quaternion(QuaternionType quater_type, shared_ptr<TableEntry> result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right):
	quater_type_(quater_type), result_(result), opA_(left), opB_(right)
{
}

string Quaternion::toString() {
	stringstream ss;

	switch (quater_type_) {
	case QuaternionType::FuncDeclareHead:
		ss << ValuetypeToString(result_->value_type()) << " " << result_->identifier();
		break;
	case QuaternionType::FuncFormalParam:
		ss << "para " << ValuetypeToString(result_->value_type()) << " " << result_->identifier();
		break;
	case QuaternionType::FuncReturn:
		if (result_.get()) {
			ss << "return " << result_->identifier();
		} else {
			ss << "return ";
		}
		break;
	case QuaternionType::FuncParamPush:
		ss << "push " << opA_->identifier();
		break;
	case QuaternionType::FuncCall:
		ss << "call " << opA_->identifier();
		break;
	case QuaternionType:: RetAssign:
		ss << result_->identifier() << " = RET";
		break;
	case QuaternionType::VarDeclare:
		/*由于数组初始化输出有点麻烦，暂时不输出涉及初始化的中间代码*/
		ss << "var " << ValuetypeToString(result_->value_type()) << " " << result_->identifier();
		//if (left_) {
		//	ss << " = " << left_->identifier();
		//}
		break;
	case QuaternionType::ConstDecalre: {
		string val = dynamic_pointer_cast<ConstEntry>(result_)->getValue();
		ss << "const " << ValuetypeToString(result_->value_type()) << " " << result_->identifier() << " = " << val;
		break;
	}
	case QuaternionType::BEQ:
		ss << opA_->identifier() << " == " << opB_->identifier() << ", GOTO " << result_->identifier();
		break;
	case QuaternionType::BNE:
		ss << opA_->identifier() << " != " << opB_->identifier() << ", GOTO " << result_->identifier();
		break;
	case QuaternionType::BLT:
		ss << opA_->identifier() << " < " << opB_->identifier() << ", GOTO " << result_->identifier();
		break;
	case QuaternionType::BLE:
		ss << opA_->identifier() << " <= " << opB_->identifier() << ", GOTO " << result_->identifier();
		break;
	case QuaternionType::BGT:
		ss << opA_->identifier() << " > " << opB_->identifier() << ", GOTO " << result_->identifier();
		break;
	case QuaternionType::BGE:
		ss << opA_->identifier() << " >= " << opB_->identifier() << ", GOTO " << result_->identifier();
		break;
	case QuaternionType::Goto:
		ss << "GOTO " << result_->identifier();
		break;
	//case QuaternionType::Bnz:
	//	ss << "BNZ " << left_->identifier();
	//	break;
	//case QuaternionType::Bz:
	//	ss << "BZ " << left_->identifier();
	//	break;
	case QuaternionType::Label:
		ss << result_->identifier() << " :";
		break;
	case QuaternionType::GetArrayElem:
		ss << result_->identifier() << " = " << opA_->identifier() << "[" << opB_->identifier() << "]";
		break;
	case QuaternionType::SetArrayELem:
		ss << result_->identifier() << "[" << opA_->identifier() << "]" << " = " << opB_->identifier();
		break;
	case QuaternionType::AddOp:
		ss << result_->identifier() << " = " << opA_->identifier() << " + " << opB_->identifier();
		break;	
	case QuaternionType::SubOp:
		ss << result_->identifier() << " = " << opA_->identifier() << " - " << opB_->identifier();
		break;	
	case QuaternionType::MulOp:
		ss << result_->identifier() << " = " << opA_->identifier() << " * " << opB_->identifier();
		break;	
	case QuaternionType::DivOp:
		ss << result_->identifier() << " = " << opA_->identifier() << " / " << opB_->identifier();
		break;
	case QuaternionType::Neg:
		ss << result_->identifier() << " =  - " << opA_->identifier();
		break;
	case QuaternionType::Assign:
		ss << result_->identifier() << " = " << opA_->identifier();
		break;
	case QuaternionType::Read:
		ss << "scanf " << result_->identifier();
		break;
	case QuaternionType::Write:
		ss << "print ";
		if (opA_) {
			ss << "\"" << opA_->getValue() << "\"" << " ";
		}
		if (opB_) {
			ss << opB_->identifier();
		}
		break;
	default:
		ss << "Some error occur in Quaternion::string";
	}
	return ss.str();
}

/******************************* QuaternionFactory ************************************/

shared_ptr<Quaternion> QuaternionFactory::FuncDeclareHead(shared_ptr<TableEntry> func) {
	return make_shared<Quaternion>(QuaternionType::FuncDeclareHead, func, nullptr, nullptr);
}

shared_ptr<Quaternion> QuaternionFactory::FuncFormalParam(shared_ptr<TableEntry> var) {
	return make_shared<Quaternion>(QuaternionType::FuncFormalParam, var, nullptr, nullptr);
}

shared_ptr<Quaternion> QuaternionFactory::FuncReturn(shared_ptr<TableEntry> var) {
	return make_shared<Quaternion>(QuaternionType::FuncReturn, var, nullptr, nullptr);
}

shared_ptr<Quaternion> QuaternionFactory::FuncParamPush(shared_ptr<TableEntry> var) {
	return make_shared<Quaternion>(QuaternionType::FuncParamPush, nullptr, var, nullptr);
}

shared_ptr<Quaternion> QuaternionFactory::FuncCall(shared_ptr<TableEntry> func) {
	return make_shared<Quaternion>(QuaternionType::FuncCall, nullptr, func, nullptr);
}

shared_ptr<Quaternion> QuaternionFactory::RetAssign(shared_ptr<TableEntry> var) {
	return make_shared<Quaternion>(QuaternionType::RetAssign, var, nullptr, nullptr);
}

shared_ptr<Quaternion> QuaternionFactory::VarDecalre(shared_ptr<TableEntry> var, shared_ptr<TableEntry> immdediate) {
	return make_shared<Quaternion>(QuaternionType::VarDeclare, var, immdediate, nullptr);
}

shared_ptr<Quaternion> QuaternionFactory::ConstDeclare(shared_ptr<TableEntry> con) {
	return make_shared<Quaternion>(QuaternionType::ConstDecalre, con, nullptr, nullptr);
}


shared_ptr<Quaternion> QuaternionFactory::BEQ(shared_ptr<TableEntry>result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right) {
	return make_shared<Quaternion>(QuaternionType::BEQ, result, left, right);
}

shared_ptr<Quaternion> QuaternionFactory::BNE(shared_ptr<TableEntry>result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right) {
	return make_shared<Quaternion>(QuaternionType::BNE, result, left, right);
}

shared_ptr<Quaternion> QuaternionFactory::BLT(shared_ptr<TableEntry>result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right) {
	return make_shared<Quaternion>(QuaternionType::BLT, result, left, right);
}

shared_ptr<Quaternion> QuaternionFactory::BLE(shared_ptr<TableEntry>result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right) {
	return make_shared<Quaternion>(QuaternionType::BLE, result, left, right);
}

shared_ptr<Quaternion> QuaternionFactory::BGT(shared_ptr<TableEntry>result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right) {
	return make_shared<Quaternion>(QuaternionType::BGT, result, left, right);
}

shared_ptr<Quaternion> QuaternionFactory::BGE(shared_ptr<TableEntry>result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right) {
	return make_shared<Quaternion>(QuaternionType::BGE, result, left, right);
}

shared_ptr<Quaternion> QuaternionFactory::Goto(shared_ptr<TableEntry> label) {
	return make_shared<Quaternion>(QuaternionType::Goto, label, nullptr, nullptr);
}

//shared_ptr<Quaternion> QuaternionFactory::Bnz(shared_ptr<TableEntry> label) {
//	return make_shared<Quaternion>(QuaternionType::Bnz, nullptr, label, nullptr);
//}
//
//shared_ptr<Quaternion> QuaternionFactory::Bz(shared_ptr<TableEntry> label) {
//	return make_shared<Quaternion>(QuaternionType::Bz, nullptr, label, nullptr);
//}

shared_ptr<Quaternion> QuaternionFactory::Label(shared_ptr<TableEntry> label) {
	return make_shared<Quaternion>(QuaternionType::Label, label, nullptr, nullptr);
}

shared_ptr<Quaternion> QuaternionFactory::getArrayElem(shared_ptr<TableEntry> result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right) {
	return make_shared<Quaternion>(QuaternionType::GetArrayElem, result, left, right);
}

shared_ptr<Quaternion> QuaternionFactory::setArrayElem(shared_ptr<TableEntry> result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right) {
	return make_shared<Quaternion>(QuaternionType::SetArrayELem, result, left, right);
}

shared_ptr<Quaternion> QuaternionFactory::Add(shared_ptr<TableEntry> result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right) {
	return make_shared<Quaternion>(QuaternionType::AddOp, result, left, right);
}

shared_ptr<Quaternion> QuaternionFactory::Sub(shared_ptr<TableEntry> result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right) {
	return make_shared<Quaternion>(QuaternionType::SubOp, result, left, right);
}

shared_ptr<Quaternion> QuaternionFactory::Mul(shared_ptr<TableEntry> result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right) {
	return make_shared<Quaternion>(QuaternionType::MulOp, result, left, right);
}

shared_ptr<Quaternion> QuaternionFactory::Div(shared_ptr<TableEntry> result, shared_ptr<TableEntry> left, shared_ptr<TableEntry> right) {
	return make_shared<Quaternion>(QuaternionType::DivOp, result, left, right);
}

shared_ptr<Quaternion> QuaternionFactory::Neg(shared_ptr<TableEntry> result, shared_ptr<TableEntry> left) {
	return make_shared<Quaternion>(QuaternionType::Neg, result, left, nullptr);
}

shared_ptr<Quaternion> QuaternionFactory::Assign(shared_ptr<TableEntry> result, shared_ptr<TableEntry> left) {
	return make_shared<Quaternion>(QuaternionType::Assign, result, left, nullptr);
}

shared_ptr<Quaternion> QuaternionFactory::Read(shared_ptr<TableEntry> result) {
	return make_shared<Quaternion>(QuaternionType::Read, result, nullptr, nullptr);
}

shared_ptr<Quaternion> QuaternionFactory::Write(shared_ptr<TableEntry> left, shared_ptr<TableEntry> right) {
	return make_shared<Quaternion>(QuaternionType::Write, nullptr, left, right);
}

string ValuetypeToString(ValueType value_type) {
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
