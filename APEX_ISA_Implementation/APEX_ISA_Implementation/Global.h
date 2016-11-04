#ifndef GLOBAL_H
#define GLOBAL_H

#include <string>
#include <iostream>

class Global
{
public:
	//program counter
	int PC;

	static const int ALU_FLAG_COUNT = 3;
	static const int FORWARDING_BUSES = 4;
	static const int ARCH_REGISTER_COUNT = 16;
	static const int MEMORY_SIZE = 4000;

	enum OPCODE{
		//ALU Instructions
		ADD,
		ADDL, //literal
		SUB,
		SUBL, //literal
		MOVC,
		MUL,
		MULL, //literal
		AND,
		ANDL, //literal
		OR,
		ORL,  //literal
		EX_OR,
		EX_ORL, //literal
		LOAD,
		STORE,
		//Control instructions
		BZ,
		BNZ,
		JUMP,
		BAL,
		HALT,
		NA
	};

	enum FLAGS{
		ZERO,
		OVER_FLOW,
		UNDER_FLOW,
		NA
	};

	enum ARCH_REGISTERS{
		R0, R1, R2, R3,
		R4, R5, R6, R7,
		R8, R9, R10, R11,
		R12, R13, R14, R15,
		NA
	};

	enum FORWARD_TYPE{
		FROM_ALU2,
		FROM_BRANCH,
		FROM_MEMORY,
		FROM_WRITEBACK
	};

	struct Register_Info{
		ARCH_REGISTERS reg_tag;
		unsigned int reg_value;
	};

	struct apexStruct{
		unsigned int pc_value = -1;
		struct instruction{
			//instruction operation code
			OPCODE op_code = OPCODE::NA;

			//destination register information
			bool destination_staus = 0;
			ARCH_REGISTERS destination_tag = ARCH_REGISTERS::NA;
			unsigned int destination_value = -1;

			//source 1 register information
			bool src1_valid = 0;
			ARCH_REGISTERS src1_tag = ARCH_REGISTERS::NA;
			unsigned int src1_value = -1;

			//source 2 register information
			bool src2_valid = 0;
			ARCH_REGISTERS src2_tag = ARCH_REGISTERS::NA;
			unsigned int src2_value = -1;

			//literal value information
			int literal_value = 0;

			//memory location for load / store
			unsigned int memory_location = 0;
		}instruction;
	};

private:
};

#endif //GLOBAL_H