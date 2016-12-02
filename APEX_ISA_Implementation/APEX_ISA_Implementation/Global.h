#ifndef GLOBAL_H
#define GLOBAL_H

#include <string>
#include <iostream>
#include <fstream>

using namespace std;

class Global
{
public:
	//constants
	//static const int ALU_FLAG_COUNT = 3;
	//static const int FORWARDING_BUSES = 4;
	//static const int ARCH_REGISTER_COUNT = 16;
	//static const int REGISTER_COUNT = 32;
	static const int MEMORY_SIZE = 4000;
	//static const int TOTAL_STAGES = 8;
	static const int ROB_SIZE = 40;

	//register status flags
	enum STATUS{
		INVALID,
		VALID,
		FINAL_STATUS_TOTAL
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
		HALT,	NONE,
		FINAL_OPCODE_TOTAL
	};

	//ALU flags
	enum FLAGS{
		ZERO,
		OVER_FLOW,
		UNDER_FLOW,
		CLEAR,
		FINAL_FLAGS_TOTAL
	};

	//available architecture registers
	enum ARCH_REGISTERS{
		R0, R1, R2, R3,
		R4, R5, R6, R7,
		R8, R9, R10, R11,
		R12, R13, R14, R15,
		X,
		FINAL_ARCH_REGISTERS_ITEM,
		NA
	};

	//unified registers
	enum REGISTERS{
		U0, U1, U2, U3, U4, 
		U5, U6, U7, U8, U9, 
		U10, U11, U12, U13, U14, 
		U15, U16, U17, U18, U19, 
		U20, U21, U22, U23, U24, 
		U25, U26, U27, U28, U29,
		U30, U31, U32,
		FINAL_REGISTERS_TOTAL,
		UX,
		UNA
	};

	//forwarding types
	enum FORWARD_TYPE{
		FROM_ALU2,
		FROM_BRANCH,
		FROM_MULTIPLY,
		FROM_MEMORY,
		FROM_WRITEBACK,
		FINAL_FORWARD_TYPE_TOTAL
	};

	//stages that can stall
	enum STALLED_STAGE{
		FETCH,
		DECODE_RF,
		ALU1,
		ALU2,
		BRANCH,
		DELAY,
		MULTIPLY,
		MEMORY,
		WRITEBACK,
		FINAL_STALLED_STAGE_TOTAL
	};

	//ROB entry allocation status
	enum ROB_ALLOCATION{
		//ROB entry unallocated
		ROB_UNALLOCATED,
		//ROB entry waiting for execution
		WAITING,
		//ROB entry executing
		EXECUTING,
		//ROB entry completed executing
		COMPLETE,
		FINAL_ROB_ALLOCATION_TOTAL
	};

	enum REGISTER_ALLOCATION{
		//register is unallocated
		REG_UNALLOCATED,
		//register is allocated, but not committed
		ALLOC_NO_COMMIT,
		//register is allocated, and committed
		ALLOC_COMMIT,
		FINAL_REGISTER_ALLOCATION_TOTAL
	};

	//type of instruction in ROB
	enum INSTRUCTION_TYPE{
		REG_TO_REG_TYPE,
		LOAD_TYPE,
		STORE_TYPE,
		BRANCH_TYPE,
		NONE_TYPE,
		FINAL_INSTRUCTION_TYPE_TOTAL
	};

	enum SOURCES{
		REGISTER_FILE,
		ROB,
		FINAL_SOURCES_TOTAL
	};

	//generic register information
	struct Register_Info{
		int tag = -1;
		unsigned int value = -1;
		REGISTER_ALLOCATION status = REGISTER_ALLOCATION::REG_UNALLOCATED;
		
		void clear()
		{
			tag = -1;
			value = -1;
			status = REGISTER_ALLOCATION::REG_UNALLOCATED;
		}
	};

	//structure used in forwarding bus
	struct Forwarding_Info{
		int pc_value = 0;
		REGISTERS tag = REGISTERS::UNA;
		Register_Info reg_info;
		//flags
		FLAGS flag = FLAGS::CLEAR;
		bool updatePC = false;
		int target = 0;
		OPCODE opcode = OPCODE::NONE;

		void clear()
		{
			pc_value = 0;
			reg_info.clear();
			flag = FLAGS::CLEAR;
			updatePC = false;
			target = 0;
			opcode = OPCODE::NONE;
		}
	};

	struct Source_Struct{
		int value = -1;
		STATUS status = STATUS::INVALID;
		int tag = -1;
		int rob_loc = -1;

		void clear()
		{
			value = -1;
			status = STATUS::INVALID;
			tag = -1;
			rob_loc = -1;
		}
	};

	//ROB entry
	struct ROB_Entry{
		int pc_value = -1;
		INSTRUCTION_TYPE type = INSTRUCTION_TYPE::NONE_TYPE;
		int destReg = -1;
		ARCH_REGISTERS destArchReg = ARCH_REGISTERS::NA;
		int result = -1;
		FLAGS flags = FLAGS::CLEAR;
		ROB_ALLOCATION alloc = ROB_ALLOCATION::ROB_UNALLOCATED;
		int saved_RAT_entry = -1;
		
		void clear()
		{
			pc_value = -1;
			type = INSTRUCTION_TYPE::NONE_TYPE;
			destReg = -1;
			destArchReg = ARCH_REGISTERS::NA;
			result = -1;
			flags = FLAGS::CLEAR;
			alloc = ROB_ALLOCATION::ROB_UNALLOCATED;
			saved_RAT_entry = -1;
		}
	};

	//Reorder Buffer (ROB)
	struct Reorder_Buffer{
		//head pointer of ROB
		int head = 0;

		//tail pointer of ROB
		int tail = 0;

		ROB_Entry entries[ROB_SIZE];
	};

	//rename table containing the current stand-in for an
	//input architectural register is a register within the
	//ARF (src bit = 0) or a slot within the ROB (src_bit=1)
	struct Rename_Table{
		int reg[ARCH_REGISTERS::FINAL_ARCH_REGISTERS_ITEM];
		int rob_loc[ARCH_REGISTERS::FINAL_ARCH_REGISTERS_ITEM];
		SOURCES src_bit[ARCH_REGISTERS::FINAL_ARCH_REGISTERS_ITEM];

		void clear()
		{
			for (int x = 0; x < ARCH_REGISTERS::FINAL_ARCH_REGISTERS_ITEM; x++)
			{
				reg[x] = -1;
				rob_loc[x] = -1;
				src_bit[x] = SOURCES::REGISTER_FILE;
			}
		}
	};

	//APEX structure to get passed between stages
	struct apexStruct{
		int pc_value = INT_MAX;
		string untouched_instruction = "";
		int branch_waiting_to_complete = INT_MAX; //instruction that branch is dependent on
		INSTRUCTION_TYPE type = INSTRUCTION_TYPE::NONE_TYPE;
		struct instruction{
			//instruction operation code
			OPCODE op_code = OPCODE::NONE;

			//destination register information
			Source_Struct dest;
			
			//source 1 register information
			Source_Struct src1;
			
			//source 2 register information
			Source_Struct src2;
			
			//literal value information
			int literal_value = 0;

			//memory location for load / store
			unsigned int memory_location = 0;

			//flags
			FLAGS flag = FLAGS::CLEAR;
		}instruction;

		void clear()
		{
			pc_value = INT_MAX;
			untouched_instruction = "";
			branch_waiting_to_complete = INT_MAX;
			type = INSTRUCTION_TYPE::NONE_TYPE;
			instruction.op_code = OPCODE::NONE;
			instruction.dest.clear();
			instruction.src1.clear();
			instruction.src2.clear();
			instruction.literal_value = 0;
			instruction.memory_location = 0;
			instruction.flag = FLAGS::CLEAR;
		}
	};

	//variables
	
	int PC; //program counter

	//functions
	static void setOutFile(string filename);
	static string getOutFile();
	static void closeFile();
	static void Debug(string s1);

	//used for debug - set enums to string for easier debugging and output
	static string toString(OPCODE opcode);
	static string toString(FLAGS flag);
	//static string toString(ARCH_REGISTERS reg);
	static string toString(REGISTERS reg);
	static string toString(FORWARD_TYPE frwd);
	static string toString(STATUS stat);
	static string toString(ROB_ALLOCATION stat);
	static string toString(REGISTER_ALLOCATION stat);
	static string toString(STALLED_STAGE stage);

private:
};

#endif //GLOBAL_H