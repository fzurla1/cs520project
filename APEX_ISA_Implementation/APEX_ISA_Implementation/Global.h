#include <string>
#include <iostream>

//program counter
int PC;

//flags from ALU
bool flags[3];

//Register file
Register_Info register_file[16];

//forwarding bus
Register_Info forward_bus[8];

//Memory - 4 bytes wide, 0 to 3999
unsigned int MEMORY[4000];

enum TYPE_INSTRUCTION{
	//ALU Instructions
	ADD,
	SUB,
	MOVC,
	MUL,
	AND,
	OR,
	EX_OR,
	LOAD,
	STORE,
	//Control instructions
	BZ,
	BNZ,
	JUMP,
	BAL,
	HALT
};

enum ARCH_REGISTERS{
	R0,	 R1,  R2,  R3,
	R4,	 R5,  R6,  R7,
	R8,  R9,  R10, R11,
	R12, R13, R14, R15
};

enum FLAGS{
	ZERO,
	OVER_FLOW,
	UNDER_FLOW
};

struct apexStruct{
	unsigned int pc_value;
	struct instruction{
		//instruction operation code
		TYPE_INSTRUCTION op_code;
		//destination register information
		ARCH_REGISTERS destination_reg;
		bool destination_staus = false;
		//source 1 register information
		bool src1_valid = false;
		ARCH_REGISTERS source1_reg;
		int src1_value;
		//source 2 register information
		bool src2_valid = false;
		ARCH_REGISTERS source2_reg;
		int src2_value;
		//literal value information
		int literal_value;
	};
};

struct Register_Info{
	ARCH_REGISTERS reg_tag;
	int reg_value;
};