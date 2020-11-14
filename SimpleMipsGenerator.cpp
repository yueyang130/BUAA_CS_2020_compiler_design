#include "SimpleMipsGenerator.h"
#include "simple_mips_code_tools.h"

// 将此全局变量作用域局限在本文件中
static const unsigned int MAXLEN = 100;
static char buf[MAXLEN];

MipsGenerator::MipsGenerator(IMCode& im_code): im_code_(im_code)
{
}

MipsGenerator& MipsGenerator::getInstance(IMCode& im_code) {
	static MipsGenerator instance(im_code);
	return instance;
}


void MipsGenerator::showMipsCode(ostream& fout) {
	for (auto x : MipsCode_list_) {
		fout << x << endl;
	}
}

void MipsGenerator::generateMipsCode() {
	dump_global_and_strcon();
	dump_main();
}

void MipsGenerator::dump_global_and_strcon() {
	vector<string> small_var_list;
	vector<string> big_var_list;
	vector<string> str_list;

	// global var
	for (auto& quater : im_code_.global_bblock().get_quater_list()) {
		if (quater->quater_type_ == QuaternionType::VarDeclare) {
			VarEntry& var = *dynamic_pointer_cast<VarEntry>(quater->result_);
			if (var.isArray()) {
				big_var_list.push_back(load_global_big_var(*quater));
			} else {
				auto instrs = load_global_small_var(*quater, this->gb_small_var_offset_map_, &_pglobal, GP_ADDR);
				small_var_list.insert(small_var_list.end(), instrs.begin(), instrs.end());
			}
		}
	}

	// string
	for (auto& quater : im_code_.get_quater_list()) {
		dump_strcon(quater->result_, str_list);
		dump_strcon(quater->opA_, str_list);
		dump_strcon(quater->opB_, str_list);
	}

	// add to mips_list
	sprintf(buf, ".data %08x", GP_ADDR);
	MipsCode_list_.push_back(buf);
	MipsCode_list_.insert(MipsCode_list_.end(), small_var_list.begin(), small_var_list.end());
	sprintf(buf, ".data %08x", DATA_ADDR);
	MipsCode_list_.push_back(buf);
	MipsCode_list_.insert(MipsCode_list_.end(), big_var_list.begin(), big_var_list.end());
	MipsCode_list_.insert(MipsCode_list_.end(), str_list.begin(), str_list.end());
	MipsCode_list_.push_back(".text");

}

void MipsGenerator::dump_main() {
	SimpleMipsFunctionGenerator main_generator(im_code_.main(), gb_small_var_offset_map_, MipsCode_list_);;
}

void dump_strcon(shared_ptr<TableEntry> p_entry, vector<string>& instr_list) {
	if (!p_entry) {
		return;
	}
	if (p_entry->entry_type() != EntryType::IMMEDIATE) {
		return;
	}
	if (p_entry->value_type() != ValueType::STRINGV) {
		return;
	}
	auto& inum = *dynamic_pointer_cast<ImmediateEntry>(p_entry);
	string instr = load_strcon(inum);
	instr_list.push_back(instr);
}
