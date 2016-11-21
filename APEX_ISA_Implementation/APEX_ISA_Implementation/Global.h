#ifndef GLOBAL_H
#define GLOBAL_H

#include <string>
#include <iostream>
#include <fstream>

using namespace std;

class Global
{
public:

	//register status flags
	enum STATUS{
		INVALID,
		VALID
	};

	//possible instruction OPCODEs
	enum OPCODE{
		//ALU Instructions
		ADD,	ADDL, //literal
		SUB,	SUBL, //literal
		MOVC,	
		MUL,	MULL, //literal
		AND,	ANDL, //literal
		OR,		ORL,  //literal
		EX_OR,	EX_ORL, //literal
		LOAD,	STORE,
		//Control instructions
		BZ,		BNZ,
		JUMP,	BAL,
		HALT,	NONE
	};

	//ALU flags
	enum FLAGS{
		ZERO,
		OVER_FLOW,
		UNDER_FLOW,
		CLEAR
	};

	//available architecture registers
	enum ARCH_REGISTERS{
		R0, R1, R2, R3,
		R4, R5, R6, R7,
		R8, R9, R10, R11,
		R12, R13, R14, R15,
		X,NA
	};

	//forwarding types
	enum FORWARD_TYPE{
		FROM_ALU2,
		FROM_BRANCH,
		FROM_MEMORY,
		FROM_WRITEBACK
	};

	//stages that can stall
	enum STALLED_STAGE{
		FETCH,
		DECODE_RF,
		ALU1,
		ALU2,
		BRANCH,
		DELAY,
		MEMORY,
		WRITEBACK
	};

	//generic register information
	struct Register_Info{
		ARCH_REGISTERS tag = ARCH_REGISTERS::NA;
		unsigned int value = -1;
		STATUS status = STATUS::INVALID;
		
		void clear()
		{
			ARCH_REGISTERS tag = ARCH_REGISTERS::NA;
			unsigned int value = -1;
			STATUS status = STATUS::INVALID;
		}
	};

	//structure used in forwarding bus
	struct Forwarding_Info{
		int pc_value = 0;
		Register_Info reg_info;
		//flags
		FLAGS flag = FLAGS::CLEAR;
		bool updatePC = false;
		int target = 0;
		OPCODE opcode = OPCODE::NONE;

		void clear()
		{
			int pc_value = 0;
			Register_Info reg_info;
			FLAGS flag = FLAGS::CLEAR;
			bool updatePC = false;
			int target = 0;
			OPCODE opcode = OPCODE::NONE;
		}
	};

	//APEX structure to get passed between stages
	struct apexStruct{
		int pc_value = INT_MAX;
		string untouched_instruction = "";
		struct instruction{
			//instruction operation code
			OPCODE op_code = OPCODE::NONE;

			//destination register information
			Register_Info dest;
			
			//source 1 register information
			Register_Info src1;
			
			//source 2 register information
			Register_Info src2;
			
			//literal value information
			int literal_value = 0;

			//memory location for load / store
			unsigned int memory_location = 0;

			//flags
			FLAGS flag = FLAGS::CLEAR;
		}instruction;

		void clear()
		{
			unsigned int pc_value = -1;
			string untouched_instruction = "";
			struct instruction{
				//instruction operation code
				OPCODE op_code = OPCODE::NONE;

				//destination register information
				Register_Info dest;

				//source 1 register information
				Register_Info src1;
		
				//source 2 register information
				Register_Info src2;
				
				//literal value information
				int literal_value = 0;

				//memory location for load / store
				unsigned int memory_location = 0;
			}instruction;
		}
	};

	//variables
	
	int PC; //program counter

	//constants
	static const int ALU_FLAG_COUNT = 3;
	static const int FORWARDING_BUSES = 4;
	static const int ARCH_REGISTER_COUNT = 16;
	static const int MEMORY_SIZE = 4000;
	static const int TOTAL_STAGES = 8;

	//functions
	static void setOutFile(string filename);
	static void closeFile();
	static void Debug(string s1);

	//used for debug - set enums to string for easier debugging and output
	static string toString(OPCODE opcode);
	static string toString(FLAGS flag);
	static string toString(ARCH_REGISTERS reg);
	static string toString(FORWARD_TYPE frwd);
	static string toString(STATUS stat);
	static string toString(STALLED_STAGE stage);

private:
};

#endif //GLOBAL_H