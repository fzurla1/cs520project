#ifndef GLOBAL_H
#define GLOBAL_H

#include <string>
#include <iostream>
#include <fstream>

using namespace std;

class Global
{
public:
	enum STATUS{
		INVALID,
		VALID
	};

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

	enum FLAGS{
		ZERO,
		OVER_FLOW,
		UNDER_FLOW,
		CLEAR
	};

	enum ARCH_REGISTERS{
		R0, R1, R2, R3,
		R4, R5, R6, R7,
		R8, R9, R10, R11,
		R12, R13, R14, R15,
		X,NA
	};

	enum FORWARD_TYPE{
		FROM_ALU2,
		FROM_BRANCH,
		FROM_MEMORY,
		FROM_WRITEBACK
	};

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

	struct apexStruct{
		int pc_value = INT_MAX;
		string untouched_instruction = "";
		struct instruction{
			//instruction operation code
			OPCODE op_code = OPCODE::NONE;

			//destination register information
			Register_Info dest;
			
			/*
			Global::STATUS destination_staus = STATUS::INVALID;
			ARCH_REGISTERS dest.tag = ARCH_REGISTERS::NA;
			int destination_value = -1;
			*/

			//source 1 register information
			Register_Info src1;
			/*
			Global::STATUS src1.status = STATUS::INVALID;
			ARCH_REGISTERS src1.tag = ARCH_REGISTERS::NA;
			int src1.value = -1;
			*/

			//source 2 register information
			Register_Info src2;
			/*
			Global::STATUS src2.status = STATUS::INVALID;
			ARCH_REGISTERS src2.tag = ARCH_REGISTERS::NA;
			int src2.value = -1;
			*/

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

				/*
				Global::STATUS destination_staus = STATUS::INVALID;
				ARCH_REGISTERS dest.tag = ARCH_REGISTERS::NA;
				int destination_value = -1;
				*/

				//source 1 register information
				Register_Info src1;
				/*
				Global::STATUS src1.status = STATUS::INVALID;
				ARCH_REGISTERS src1.tag = ARCH_REGISTERS::NA;
				int src1.value = -1;
				*/

				//source 2 register information
				Register_Info src2;
				/*
				Global::STATUS src2.status = STATUS::INVALID;
				ARCH_REGISTERS src2.tag = ARCH_REGISTERS::NA;
				int src2.value = -1;
				*/

				//literal value information
				int literal_value = 0;

				//memory location for load / store
				unsigned int memory_location = 0;
			}instruction;
		}
	};

	//variables
	
	int PC; //program counter

	static const int ALU_FLAG_COUNT = 3;
	static const int FORWARDING_BUSES = 4;
	static const int ARCH_REGISTER_COUNT = 16;
	static const int MEMORY_SIZE = 4000;
	static const int TOTAL_STAGES = 8;

	//functions
	static void setOutFile(string filename);
	static void closeFile();
	static void Debug(string s1);
	static string toString(OPCODE opcode);
	static string toString(FLAGS flag);
	static string toString(ARCH_REGISTERS reg);
	static string toString(FORWARD_TYPE frwd);
	static string toString(STATUS stat);
	static string toString(STALLED_STAGE stage);

private:
};

#endif //GLOBAL_H