#include "stdafx.h"
#include "Global.h"

#define DEBUG 0

using namespace std;

//main output
ofstream fout;
string output_file_name;

//ROB output
ofstream fout_ROB;
string output_file_name_ROB;

//FRAT output
ofstream fout_FRAT;
string output_file_name_FRAT;

//BRAT output
ofstream fout_BRAT;
string output_file_name_BRAT;

void Global::setOutFiles(string filename, string filename_ROB, string filename_FRAT, string filename_BRAT)
{
	output_file_name = filename;
	output_file_name_ROB = filename_ROB;
	output_file_name_FRAT = filename_FRAT;
	output_file_name_BRAT = filename_BRAT;
	
	fout.open(filename);
	fout_ROB.open(filename_ROB, ios_base::out | ios_base::trunc);
	fout_FRAT.open(filename_FRAT, ios_base::out | ios_base::trunc);
	fout_BRAT.open(filename_BRAT, ios_base::out | ios_base::trunc);

	if (!fout)
	{
		cout << "Failed to open file: " + filename << endl;
	}
	else
	{
		cout << "File " + filename + " opened successfully." << endl;
	}

	if (!fout_ROB)
	{
		cout << "Failed to open file: " + filename_ROB << endl;
	}
	else
	{
		cout << "File " + filename_ROB + " opened successfully." << endl;
	}

	if (!fout_FRAT)
	{
		cout << "Failed to open file: " + filename_FRAT << endl;
	}
	else
	{
		cout << "File " + filename_FRAT + " opened successfully." << endl;
	}

	if (!fout_BRAT)
	{
		cout << "Failed to open file: " + filename_BRAT << endl;
	}
	else
	{
		cout << "File " + filename_BRAT + " opened successfully." << endl;
	}
}

string Global::getOutFile()
{
	return output_file_name;
}

string Global::getOutFileROB()
{
	return output_file_name_ROB;
}

string Global::getOutFileFRAT()
{
	return output_file_name_FRAT;
}

string Global::getOutFileBRAT()
{
	return output_file_name_BRAT;
}

bool Global::fileIsGood()
{
	return fout.is_open() & fout.good() 
		& fout_ROB.is_open() & fout_ROB.good() 
		& fout_FRAT.is_open() & fout_FRAT.good() 
		& fout_BRAT.is_open() & fout_BRAT.good();
}

void Global::closeFiles()
{
	fout.close();
	fout_ROB.close();
	fout_FRAT.close();
	fout_BRAT.close();
}

void Global::Output(string s1)
{
	if (DEBUG)
		cout << s1 << endl;

	fout << s1 << endl;
}

void Global::OutputROB(string s1)
{
	if (DEBUG)
		cout << s1 << endl;

	fout_ROB << s1 << endl;
}

void Global::OutputFRAT(string s1)
{
	if (DEBUG)
		cout << s1 << endl;

	fout_FRAT << s1 << endl;
}

void Global::OutputBRAT(string s1)
{
	if (DEBUG)
		cout << s1 << endl;

	fout_BRAT << s1 << endl;
}

//OpCodes
string Global::toString(OPCODE opcode)
{
	string out = "";
	switch (opcode)
	{
		case OPCODE::ADD:
			out = "ADD";
			break;
		case OPCODE::ADDL:
			out = "ADDL";
			break;
		case OPCODE::BAL:
			out = "BAL";
			break;
		case OPCODE::BNZ:
			out = "BNZ";
			break;
		case OPCODE::BZ:
			out = "BZ";
			break;
		case OPCODE::EX_OR:
			out = "EX_OR";
			break;
		case OPCODE::EX_ORL:
			out = "EX_ORL";
			break;
		case OPCODE::HALT:
			out = "HALT";
			break;
		case OPCODE::JUMP:
			out = "JUMP";
			break;
		case OPCODE::LOAD:
			out = "LOAD";
			break;
		case OPCODE::MOVC:
			out = "MOVC";
			break;
		case OPCODE::MUL:
			out = "MUL";
			break;
		case OPCODE::MULL:
			out = "MULL";
			break;
		case OPCODE::OR:
			out = "OR";
			break;
		case OPCODE::ORL:
			out = "ORL";
			break;
		case OPCODE::STORE:
			out = "STORE";
			break;
		case OPCODE::SUB:
			out = "SUB";
			break;
		case OPCODE::SUBL:
			out = "SUBL";
			break;
		default:
			out = "NONE";
			break;
	}

	return out;
}

//Flag types
string Global::toString(FLAGS flag)
{
	string out = "";
	switch (flag)
	{
	case FLAGS::CLEAR:
		out = "CLEAR";
		break;
	case FLAGS::OVER_FLOW:
		out = "OVER FLOW";
		break;
	case FLAGS::UNDER_FLOW:
		out = "UNDER FLOW";
		break;
	case FLAGS::ZERO:
		out = "ZERO";
		break;
	default:
		out = "UNKNOWN";
		break;
	}
	return out;
}

//Architectural Register Types
string Global::toString(ARCH_REGISTERS reg)
{
	string out = "";
	switch (reg)
	{
	case ARCH_REGISTERS::R0:
		out = "R0";
		break;
	case ARCH_REGISTERS::R1:
		out = "R1";
		break;
	case ARCH_REGISTERS::R2:
		out = "R2";
		break;
	case ARCH_REGISTERS::R3:
		out = "R3";
		break;
	case ARCH_REGISTERS::R4:
		out = "R4";
		break;
	case ARCH_REGISTERS::R5:
		out = "R5";
		break;
	case ARCH_REGISTERS::R6:
		out = "R6";
		break;
	case ARCH_REGISTERS::R7:
		out = "R7";
		break;
	case ARCH_REGISTERS::R8:
		out = "R8";
		break;
	case ARCH_REGISTERS::R9:
		out = "R9";
		break;
	case ARCH_REGISTERS::R10:
		out = "R10";
		break;
	case ARCH_REGISTERS::R11:
		out = "R11";
		break;
	case ARCH_REGISTERS::R12:
		out = "R12";
		break;
	case ARCH_REGISTERS::R13:
		out = "R13";
		break;
	case ARCH_REGISTERS::R14:
		out = "R14";
		break;
	case ARCH_REGISTERS::R15:
		out = "R15";
		break;
	case ARCH_REGISTERS::NA:
	default:
		out = "N/A";
		break;
		break;
	}
	return out;
}

//Registers
string Global::toString(REGISTERS reg)
{
	string out = "U" + int(reg);

	if (reg == REGISTERS::UNA)
		out = "UNA";
	else if (reg == REGISTERS::UX)
		out = "X";
	return out;
}

string Global::toString(Register_Info reg)
{
	string out = "";

	out += "Status = " + toString(reg.status) + '\n';
	out += "Tag    = " + to_string(reg.tag) + '\n';
	out += "Value  = " + to_string(reg.value) + '\n';

	return out;
}

//Forwarding Types
string Global::toString(FORWARD_TYPE frwd)
{
	string out = "";
	switch (frwd)
	{
	case FORWARD_TYPE::FROM_ALU2:
		out = "From ALU2";
		break;
	case FORWARD_TYPE::FROM_BRANCH:
		out = "From Branch";
		break;
	case FORWARD_TYPE::FROM_MEMORY:
		out = "From Memory";
		break;
	case FORWARD_TYPE::FROM_WRITEBACK:
		out = "From Writeback";
		break;
	default:
		out = "UNKNOWN";
		break;
	}
	return out;
}

//Status Types
string Global::toString(STATUS stat)
{
	string out = "";
	switch (stat)
	{
	case STATUS::INVALID:
		out = "INVALID";
		break;
	case STATUS::VALID:
		out = "VALID";
		break;
	default:
		out = "INVALID";
		break;
	}
	return out;
}

//ROB allocation status
string Global::toString(ROB_ALLOCATION stat)
{
	string out = "";
	return out;
}

//Register allocation status
string Global::toString(REGISTER_ALLOCATION stat)
{
	string out = "";
	switch (stat)
	{
		case REGISTER_ALLOCATION::ALLOC_COMMIT:
			out = "ALLOCATED and COMMITTED";
			break;
		case REGISTER_ALLOCATION::ALLOC_NO_COMMIT:
			out = "ALLOCATED and NOT COMMITTED";
			break;
		case REGISTER_ALLOCATION::REG_UNALLOCATED:
			out = "UNALLOCATED";
			break;
		default:
			break;
	}
	return out;
}

//Stalled Stages
string Global::toString(STALLED_STAGE stage)
{
	string out = "";
	switch (stage)
	{
		case STALLED_STAGE::FETCH:
			out = "FETCH";
			break;
		case STALLED_STAGE::DECODE_RF:
			out = "DECODE_RF";
			break;
		case STALLED_STAGE::ALU1:
			out = "ALU1";
			break;
		case STALLED_STAGE::ALU2:
			out = "ALU2";
			break;
		case STALLED_STAGE::BRANCH:
			out = "BRANCH";
			break;
		case STALLED_STAGE::DELAY:
			out = "DELAY";
			break;
		case STALLED_STAGE::MEMORY:
			out = "MEMORY";
			break;
		case STALLED_STAGE::WRITEBACK:
			out = "WRITEBACK";
			break;
	}

	return out;
}

string Global::toString(INSTRUCTION_TYPE type)
{
	string out = "";
	switch (type)
	{
		case INSTRUCTION_TYPE::REG_TO_REG_TYPE:
			out = "REG_TO_REG";
			break;
		case INSTRUCTION_TYPE::LOAD_TYPE:
			out = "LOAD";
			break;
		case INSTRUCTION_TYPE::STORE_TYPE:
			out = "STORE";
			break;
		case INSTRUCTION_TYPE::BRANCH_TYPE:
			out = "BRANCH";
			break;
		default:
			out = "NONE";
			break;
	}
	return out;
}

string Global::toString(ROB_Entry entry)
{
	string out = "";

	out += "Alloc    = " + toString(entry.alloc) + '\n';
	out += "Arch Reg = " + toString(entry.destArchReg) + '\n';
	out += "URF Ref  = " + to_string(entry.destReg) + '\n';
	out += "Flags    = " + toString(entry.flags) + '\n';
	out += "PC Value = " + to_string(entry.pc_value) + '\n';
	out += "Result   = " + to_string(entry.result) + '\n';
	out += "Type     = " + toString(entry.type) + '\n';

	return out;
}

string Global::toString(SOURCES src)
{
	string out = "";

	switch (src)
	{
		case SOURCES::REGISTER_FILE:
			out = "Unified Register File";
			break;
		case SOURCES::ROB:
			out = "Reorder Buffer";
			break;
		default:
			break;
	}

	return out;
}