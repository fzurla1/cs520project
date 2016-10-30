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

	//Memory - 4 bytes wide, 0 to 3999
	static unsigned int MEMORY[4000];

	enum TYPE_INSTRUCTION{
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
		HALT
	};

	enum FLAGS{
		ZERO,
		OVER_FLOW,
		UNDER_FLOW
	};

	enum ARCH_REGISTERS{
		R0, R1, R2, R3,
		R4, R5, R6, R7,
		R8, R9, R10, R11,
		R12, R13, R14, R15
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
		unsigned int pc_value;
		struct instruction{
			//instruction operation code
			TYPE_INSTRUCTION op_code;

			//destination register information
			//bool destination_staus = false;
			ARCH_REGISTERS destination_reg;
			unsigned int destination_value;

			//source 1 register information
			bool src1_valid = false;
			ARCH_REGISTERS src1_tag;
			unsigned int src1_value;

			//source 2 register information
			bool src2_valid = false;
			ARCH_REGISTERS src2_tag;
			unsigned int src2_value;

			//literal value information
			int literal_value;

			//memory location for load / store
			unsigned int memory_location;
		}instruction;
	};

private:
};

#endif //GLOBAL_H