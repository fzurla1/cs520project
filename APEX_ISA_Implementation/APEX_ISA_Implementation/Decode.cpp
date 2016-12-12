#include "stdafx.h"
#include "Decode.h"
#include <sstream>

enum DESTINATION
{
	NONE,
	ALU,
	BRANCH
};

Decode::Decode()
{
}

Decode::~Decode()
{
}

Global::apexStruct Decode::run(
	Global::Register_Info *Register_File,
	int URF_SIZE,
	Global::Forwarding_Info(&Forward_Bus)[Global::FORWARD_TYPE::FINAL_FORWARD_TYPE_TOTAL],
	bool(&Stalled_Stages)[Global::STALLED_STAGE::FINAL_STALLED_STAGE_TOTAL],
	Global::Reorder_Buffer(&ROB),
	Global::Front_End_Rename_Entry(&Front_End_RAT)[Global::ARCH_REGISTERS::FINAL_ARCH_REGISTERS_ITEM])
{
	Global::apexStruct output_struct;

	//if a branch has occured
	//if my PC is > than the branch, meaning that this instruction is later in the instruction
	//set, clear out my instruction
	if ((Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].updatePC)
		&& (Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].pc_value < myStruct.pc_value))
	{
		myStruct.clear();
		return output_struct;
	}
	output_struct = myStruct;
	snapshot_before = myStruct;

	//break down raw instruction string
	std::istringstream iss(myStruct.untouched_instruction);
	string  s_opcode = "",
		s_dest = "",
		s_reg1 = "",
		s_reg2 = "",
		s_reg3 = "",
		s_literal = "";

	int destination = NONE;
	int arch_dest = -1, 
		arch_src1 = -1,
		arch_src2 = -1;

	//assume no stall
	Stalled_Stages[Global::STALLED_STAGE::DECODE_RF] = false;

	//make sure we have valid data
	if (myStruct.pc_value != INT_MAX)
	{
		//need to decode first before we know if we are stalled
		//can send an instruction to branch if ALU1 is stalled, and vice-versa
		iss >> s_opcode;

		output_struct.instruction.op_code = Global::OPCODE::NONE;

#pragma region Decode OpCode
			switch (s_opcode[0]) //break down string
			{
#pragma region 'A' instructions
			case 'A':
				switch (s_opcode[1])
				{
				case 'D':
					if (s_opcode[3] == 'L')
					{
						output_struct.instruction.op_code = Global::OPCODE::ADDL;
					}
					else
					{
						output_struct.instruction.op_code = Global::OPCODE::ADD;
					}
					break;
				case 'N':
					if (s_opcode[3] == 'L')
					{
						output_struct.instruction.op_code = Global::OPCODE::ANDL;
					}
					else
					{
						output_struct.instruction.op_code = Global::OPCODE::AND;
					}
					break;
				default:
					break;
				}
				break;
#pragma endregion

#pragma region 'S' instructions
			case 'S':
				switch (s_opcode[1])
				{
				case 'U':
					if (s_opcode[3] == 'L')
					{
						output_struct.instruction.op_code = Global::OPCODE::SUBL;
					}
					else
					{
						output_struct.instruction.op_code = Global::OPCODE::SUB;
					}

					break;
				case 'T':
					output_struct.instruction.op_code = Global::OPCODE::STORE;
					break;
				default:
					break;
				}
				break;
#pragma endregion

#pragma region 'M' instructions
			case 'M':
				switch (s_opcode[1])
				{
				case 'O':
					output_struct.instruction.op_code = Global::OPCODE::MOVC;
					break;
				case 'U':
					if (s_opcode[3] == 'L')
					{
						output_struct.instruction.op_code = Global::OPCODE::MULL;
					}
					else
					{
						output_struct.instruction.op_code = Global::OPCODE::MUL;
					}

				default:
					break;
				}
				break;
#pragma endregion

#pragma region 'O' instructions
			case 'O':
				if (s_opcode[2] == 'L')
				{
					output_struct.instruction.op_code = Global::OPCODE::ORL;
				}
				else
				{
					output_struct.instruction.op_code = Global::OPCODE::OR;
				}

				break;
#pragma endregion

#pragma region 'E' instructions
			case 'E':
				if (s_opcode[5] == 'L')
				{
					output_struct.instruction.op_code = Global::OPCODE::EX_ORL;
				}
				else
				{
					output_struct.instruction.op_code = Global::OPCODE::EX_OR;
				}

				break;
#pragma endregion

#pragma region 'L' instructions
			case 'L':
				output_struct.instruction.op_code = Global::OPCODE::LOAD;
				break;
#pragma endregion

#pragma region 'B' instructions
			case 'B':
				if (s_opcode[1] == 'A')
				{
					output_struct.instruction.op_code = Global::OPCODE::BAL;
				}
				else if (s_opcode[1] == 'N')
				{
					output_struct.instruction.op_code = Global::OPCODE::BNZ;
				}
				else if (s_opcode[1] == 'Z')
				{
					output_struct.instruction.op_code = Global::OPCODE::BZ;
				}
				break;
#pragma endregion

#pragma region 'J' instructions
			case 'J':
				output_struct.instruction.op_code = Global::OPCODE::JUMP;
				break;
#pragma endregion

#pragma region 'H' instructions
			case 'H':
				output_struct.instruction.op_code = Global::OPCODE::HALT;
				break;
#pragma endregion

			default:
				output_struct.instruction.op_code = Global::OPCODE::NONE;
				break;
			}
#pragma endregion //Decode OpCode

		//break down the rest of the line based on the instruction type
		switch (output_struct.instruction.op_code)
		{
			//opcode <dest>, <src1>, <src2>
			case Global::OPCODE::ADD:
			case Global::OPCODE::AND:
			case Global::OPCODE::EX_OR:
			case Global::OPCODE::MUL:
			case Global::OPCODE::OR:
			case Global::OPCODE::SUB:
				iss >> s_dest;
				iss >> s_reg1;
				iss >> s_reg2;

				s_dest = s_dest.substr(1, s_dest.length() - 1);
				s_reg1 = s_reg1.substr(1, s_reg1.length() - 1);
				s_reg2 = s_reg2.substr(1, s_reg2.length());

				last_alu_mul_pc = output_struct.pc_value;

				arch_dest = atoi(s_dest.c_str());
				arch_src1 = atoi(s_reg1.c_str());
				arch_src2 = atoi(s_reg2.c_str());

				//set up source 1
				output_struct.instruction.src1.status = Global::STATUS::VALID;
				output_struct.instruction.src1.tag = Front_End_RAT[arch_src1].reg;
				output_struct.instruction.src1.archreg = Global::ARCH_REGISTERS(arch_src1);
				output_struct.instruction.src1.rob_loc = -1;

				//set up source 2
				output_struct.instruction.src2.status = Global::STATUS::VALID;
				output_struct.instruction.src2.tag = Front_End_RAT[arch_src2].reg;
				output_struct.instruction.src2.archreg = Global::ARCH_REGISTERS(arch_src2);
				output_struct.instruction.src2.rob_loc = -1;

				destination = ALU;
				output_struct.type = Global::INSTRUCTION_TYPE::REG_TO_REG_TYPE;
				break;

			//opcode <dest>, <src1>, #literal
			case Global::OPCODE::ADDL:
			case Global::OPCODE::ANDL:
			case Global::OPCODE::EX_ORL:
			case Global::OPCODE::MULL:
			case Global::OPCODE::ORL:
			case Global::OPCODE::SUBL:
			case Global::OPCODE::LOAD:
				iss >> s_dest;
				iss >> s_reg1;
				iss >> s_literal;

				s_dest = s_dest.substr(1, s_dest.length() - 1);
				s_reg1 = s_reg1.substr(1, s_reg1.length() - 1);
				s_literal = s_literal.substr(1, s_literal.length());

				last_alu_mul_pc = output_struct.pc_value;

				arch_dest = atoi(s_dest.c_str());
				arch_src1 = atoi(s_reg1.c_str());

				//set up source 1
				output_struct.instruction.src1.status = Global::STATUS::VALID;
				output_struct.instruction.src1.tag = Front_End_RAT[arch_src1].reg;
				output_struct.instruction.src1.rob_loc = -1;
				output_struct.instruction.src1.archreg = Global::ARCH_REGISTERS(arch_src1);

				//set source 2 to valid since there isnt one
				output_struct.instruction.src2.status = Global::STATUS::VALID;

				//set up literal
				output_struct.instruction.literal_value = atoi(s_literal.c_str());

				destination = ALU;
				if (output_struct.instruction.op_code != Global::OPCODE::LOAD)
				{
					output_struct.type = Global::INSTRUCTION_TYPE::REG_TO_REG_TYPE;
				}
				else
				{
					output_struct.type = Global::INSTRUCTION_TYPE::LOAD_TYPE;
				}
				break;
		
			case Global::OPCODE::STORE:
				iss >> s_reg1;
				iss >> s_reg2;
				iss >> s_literal;

				s_reg1 = s_reg1.substr(1, s_reg1.length() - 1);
				s_reg2 = s_reg2.substr(1, s_reg2.length() - 1);
				s_literal = s_literal.substr(1, s_literal.length());

				arch_src1 = atoi(s_reg1.c_str());
				arch_src2 = atoi(s_reg2.c_str());

				//set up source 1
				output_struct.instruction.src1.status = Global::STATUS::VALID;
				output_struct.instruction.src1.tag = Front_End_RAT[arch_src1].reg;
				output_struct.instruction.src1.rob_loc = -1;
				output_struct.instruction.src1.archreg = Global::ARCH_REGISTERS(arch_src1);

				//set up source 2
				output_struct.instruction.src2.status = Global::STATUS::VALID;
				output_struct.instruction.src2.tag = Front_End_RAT[arch_src2].reg;
				output_struct.instruction.src2.rob_loc = -1;
				output_struct.instruction.src2.archreg = Global::ARCH_REGISTERS(arch_src2);

				output_struct.instruction.literal_value = atoi(s_literal.c_str());

				destination = ALU;
				output_struct.type = Global::INSTRUCTION_TYPE::STORE_TYPE;
				break;

			case Global::OPCODE::BNZ:
			case Global::OPCODE::BZ:
				iss >> s_literal;
				s_literal = s_literal.substr(1, s_literal.length());
				output_struct.instruction.literal_value = atoi(s_literal.c_str());

				output_struct.instruction.dest.status = Global::STATUS::VALID;
				output_struct.instruction.src1.status = Global::STATUS::VALID;
				output_struct.instruction.src2.status = Global::STATUS::VALID;
				output_struct.branch_waiting_to_complete = last_alu_mul_pc;
				output_struct.type = Global::INSTRUCTION_TYPE::BRANCH_TYPE;
				break;
				//opcode <src1>, #literal
			case Global::OPCODE::BAL:
			case Global::OPCODE::MOVC:
				iss >> s_dest;
				s_dest = s_dest.substr(1, (s_dest.length() - 1));
				iss >> s_literal;
				s_literal = s_literal.substr(1, s_literal.length());

				arch_dest = atoi(s_dest.c_str());

				output_struct.instruction.src1.tag = -1;
				output_struct.instruction.src1.status = Global::STATUS::VALID;

				output_struct.instruction.src2.tag = -1;
				output_struct.instruction.src2.status = Global::STATUS::VALID;

				output_struct.instruction.literal_value = atoi(s_literal.c_str());

				destination = BRANCH;
				if (output_struct.instruction.op_code == Global::OPCODE::BAL)
				{
					output_struct.type = Global::INSTRUCTION_TYPE::BRANCH_TYPE;
				}
				else
				{
					output_struct.instruction.dest.archreg = Global::ARCH_REGISTERS(arch_dest);
					output_struct.type = Global::INSTRUCTION_TYPE::REG_TO_REG_TYPE;
				}
				break;
			case Global::OPCODE::JUMP:
				iss >> s_dest;
				s_dest = s_dest.substr(1, (s_dest.length() - 1));
				iss >> s_literal;
				s_literal = s_literal.substr(1, s_literal.length());

				output_struct.instruction.dest.tag = Global::ARCH_REGISTERS::NA;

				arch_src1 = Global::ARCH_REGISTERS::X;
				output_struct.instruction.src1.archreg = Global::ARCH_REGISTERS::X;
				output_struct.instruction.src1.status = Global::STATUS::VALID;

				output_struct.instruction.src2.tag = Global::ARCH_REGISTERS::NA;
				output_struct.instruction.src2.status = Global::STATUS::VALID;

				//FOR SRC1
				//look to FE RAT for where data is stored
				if ((Front_End_RAT[arch_src1].src_bit == Global::SOURCES::REGISTER_FILE)
					&& (Register_File[Front_End_RAT[arch_src1].reg].status == Global::REGISTER_ALLOCATION::ALLOC_COMMIT))
				{
					output_struct.instruction.src1.value = Register_File[output_struct.instruction.src1.tag].value;
				}

				//check ROB and forwarding buses
				//if tag matches what we are looking for, and that instruction is the most recent to update 
				//the register, then grab that value, otherwise, stall.
				else
				{
					//get ROB entry location
					output_struct.instruction.src1.rob_loc = Front_End_RAT[arch_src1].reg;

					//check ROB
					//if ROB entry is complete, get out data
					//get ROB entry # from FE RAT ROB location entry based on arch register from src1 input
					if (ROB.entries[Front_End_RAT[arch_src1].rob_loc].alloc == Global::ROB_ALLOCATION::COMPLETE)
					{
						output_struct.instruction.src1.value = ROB.entries[Front_End_RAT[arch_src1].rob_loc].result;
					}
					//if the entry is not complete, but it is executing or waiting, check forwarding bus
					else
					{
						//check from ALU2
						if (Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].reg_info.tag == output_struct.instruction.src1.tag)
						{
							output_struct.instruction.src1.value = Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].reg_info.value;
						}
						//check from memory
						else if (Forward_Bus[Global::FORWARD_TYPE::FROM_MULTIPLY].reg_info.tag == output_struct.instruction.src1.tag)
						{
							output_struct.instruction.src1.value = Forward_Bus[Global::FORWARD_TYPE::FROM_MULTIPLY].reg_info.value;
						}
						//check from writeback
						else if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK1].reg_info.tag == output_struct.instruction.src1.tag)
						{
							output_struct.instruction.src1.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK1].reg_info.value;
						}
						//check from writeback
						else if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK2].reg_info.tag == output_struct.instruction.src1.tag)
						{
							output_struct.instruction.src1.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK2].reg_info.value;
						}
						//check from writeback
						else if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK3].reg_info.tag == output_struct.instruction.src1.tag)
						{
							output_struct.instruction.src1.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK3].reg_info.value;
						}
						//check from writeback
						else if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK4].reg_info.tag == output_struct.instruction.src1.tag)
						{
							output_struct.instruction.src1.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK4].reg_info.value;
						}
						else
						{
							output_struct.instruction.src1.status = Global::STATUS::INVALID;
						}
					}
				}

				output_struct.instruction.literal_value = atoi(s_literal.c_str());

				destination = BRANCH;
				output_struct.type = Global::INSTRUCTION_TYPE::BRANCH_TYPE;
				break;
			default:
				break;

		}
		//set source 1 info
		switch (output_struct.instruction.op_code)
		{
			case Global::OPCODE::ADD:
			case Global::OPCODE::ADDL:
			case Global::OPCODE::AND:
			case Global::OPCODE::ANDL:
			case Global::OPCODE::EX_OR:
			case Global::OPCODE::EX_ORL:
			case Global::OPCODE::MUL:
			case Global::OPCODE::MULL:
			case Global::OPCODE::OR:
			case Global::OPCODE::ORL:
			case Global::OPCODE::SUB:
			case Global::OPCODE::SUBL:
			case Global::OPCODE::STORE:
			case Global::OPCODE::LOAD:
				//FOR SRC1
				//if the entry is not complete, but it is executing or waiting, check forwarding bus
				//check from ALU2
				if (Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].reg_info.tag == output_struct.instruction.src1.tag)
				{
					output_struct.instruction.src1.value = Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].reg_info.value;
				}
				//check from memory
				else if (Forward_Bus[Global::FORWARD_TYPE::FROM_MULTIPLY].reg_info.tag == output_struct.instruction.src1.tag)
				{
					output_struct.instruction.src1.value = Forward_Bus[Global::FORWARD_TYPE::FROM_MULTIPLY].reg_info.value;
				}
				//check from writeback
				else if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK1].reg_info.tag == output_struct.instruction.src1.tag)
				{
					output_struct.instruction.src1.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK1].reg_info.value;
				}
				//check from writeback
				else if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK2].reg_info.tag == output_struct.instruction.src1.tag)
				{
					output_struct.instruction.src1.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK2].reg_info.value;
				}
				//check from writeback
				else if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK3].reg_info.tag == output_struct.instruction.src1.tag)
				{
					output_struct.instruction.src1.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK3].reg_info.value;
				}
				//check from writeback
				else if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK4].reg_info.tag == output_struct.instruction.src1.tag)
				{
					output_struct.instruction.src1.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK4].reg_info.value;
				}
				//check ROB and forwarding buses
				//if tag matches what we are looking for, and that instruction is the most recent to update 
				//the register, then grab that value, otherwise, stall.
				else if (ROB.entries[Front_End_RAT[arch_src1].rob_loc].alloc == Global::ROB_ALLOCATION::COMPLETE)
				{
					//get ROB entry location
					output_struct.instruction.src1.rob_loc = Front_End_RAT[arch_src1].reg;
					output_struct.instruction.src1.value = ROB.entries[Front_End_RAT[arch_src1].rob_loc].result;
				}
				//look to FE RAT for where data is stored
				else if ((Front_End_RAT[arch_src1].src_bit == Global::SOURCES::REGISTER_FILE)
					&& (Register_File[Front_End_RAT[arch_src1].reg].status == Global::REGISTER_ALLOCATION::ALLOC_COMMIT))
				{
					output_struct.instruction.src1.value = Register_File[output_struct.instruction.src1.tag].value;
				}
				else
				{
					output_struct.instruction.src1.status = Global::STATUS::INVALID;
				}
				break;
			default:
				break;
		}

		//set source 2 info
		switch (output_struct.instruction.op_code)
		{
			case Global::OPCODE::ADD:
			case Global::OPCODE::AND:
			case Global::OPCODE::EX_OR:
			case Global::OPCODE::MUL:
			case Global::OPCODE::OR:
			case Global::OPCODE::SUB:
			case Global::OPCODE::STORE:
				//FOR SRC2
				//if the entry is not complete, but it is executing or waiting, check forwarding bus
				//check from ALU2
				if (Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].reg_info.tag == output_struct.instruction.src2.tag)
				{
					output_struct.instruction.src2.value = Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].reg_info.value;
				}
				//check from memory
				else if (Forward_Bus[Global::FORWARD_TYPE::FROM_MULTIPLY].reg_info.tag == output_struct.instruction.src2.tag)
				{
					output_struct.instruction.src2.value = Forward_Bus[Global::FORWARD_TYPE::FROM_MULTIPLY].reg_info.value;
				}
				//check from writeback
				else if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK1].reg_info.tag == output_struct.instruction.src2.tag)
				{
					output_struct.instruction.src2.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK1].reg_info.value;
				}
				//check from writeback
				else if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK2].reg_info.tag == output_struct.instruction.src2.tag)
				{
					output_struct.instruction.src2.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK2].reg_info.value;
				}
				//check from writeback
				else if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK3].reg_info.tag == output_struct.instruction.src2.tag)
				{
					output_struct.instruction.src2.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK3].reg_info.value;
				}
				//check from writeback
				else if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK4].reg_info.tag == output_struct.instruction.src2.tag)
				{
					output_struct.instruction.src2.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK4].reg_info.value;
				}
				//check ROB and forwarding buses
				//if tag matches what we are looking for, and that instruction is the most recent to update 
				//the register, then grab that value, otherwise, stall.
				else if (ROB.entries[Front_End_RAT[arch_src2].rob_loc].alloc == Global::ROB_ALLOCATION::COMPLETE)
				{
					//get ROB entry location
					output_struct.instruction.src2.rob_loc = Front_End_RAT[arch_src1].reg;
					output_struct.instruction.src2.value = ROB.entries[Front_End_RAT[arch_src1].rob_loc].result;
				}
				//look to FE RAT for where data is stored
				else if ((Front_End_RAT[arch_src2].src_bit == Global::SOURCES::REGISTER_FILE)
					&& (Register_File[Front_End_RAT[arch_src2].reg].status == Global::REGISTER_ALLOCATION::ALLOC_COMMIT))
				{
					output_struct.instruction.src2.value = Register_File[output_struct.instruction.src2.tag].value;
				}
				else
				{
					output_struct.instruction.src2.status = Global::STATUS::INVALID;
				}
				break;
			default:
				break;
		}

		if (output_struct.instruction.op_code == Global::OPCODE::MOVC)
		{
			destination = ALU;
		}

		//set up the destination information within the register file and most recently used array
		output_struct.instruction.dest.status = Global::STATUS::INVALID;

		//set up ROB entry
		//set up destination
		//1) find a register that is unallocated
		//2) mark as allocated, but not committed
		//3) update FE RAT with register and mark source bit as ROB
		//4) create new entry in ROB
		//5) move tail
		bool entry_made = false;
		if ((output_struct.instruction.op_code != Global::OPCODE::BAL)
			&& (output_struct.type != Global::INSTRUCTION_TYPE::STORE_TYPE))
		{
			for (int x = 0; x < URF_SIZE - 1; x++) //last register is designated for X
			{
				//1
				if (Register_File[x].status == Global::REGISTER_ALLOCATION::REG_UNALLOCATED)
				{
					output_struct.instruction.dest.tag = x;
					output_struct.instruction.dest.rob_loc = ROB.tail;

					//if not a branch or store instruction, set up the register file and FE RAT
					if ((output_struct.type != Global::INSTRUCTION_TYPE::BRANCH_TYPE)
						|| (output_struct.type != Global::INSTRUCTION_TYPE::STORE_TYPE))
					{
						//2
						Register_File[x].status = Global::REGISTER_ALLOCATION::ALLOC_NO_COMMIT;
						//3
						Front_End_RAT[arch_dest].reg = x;
						Front_End_RAT[arch_dest].src_bit = Global::SOURCES::ROB;
						Front_End_RAT[arch_dest].rob_loc = ROB.tail;
					}
					//4
					ROB.entries[ROB.tail].alloc = Global::ROB_ALLOCATION::WAITING;
					ROB.entries[ROB.tail].destReg = x;
					ROB.entries[ROB.tail].destArchReg = Global::ARCH_REGISTERS(arch_dest);
					ROB.entries[ROB.tail].flags = Global::FLAGS::CLEAR;
					ROB.entries[ROB.tail].pc_value = myStruct.pc_value;
					ROB.entries[ROB.tail].result = 0;
					ROB.entries[ROB.tail].instruction = myStruct.untouched_instruction;
					if (output_struct.instruction.op_code != Global::OPCODE::LOAD)
					{
						ROB.entries[ROB.tail].type = Global::INSTRUCTION_TYPE::REG_TO_REG_TYPE;
					}
					else
					{
						ROB.entries[ROB.tail].type = Global::INSTRUCTION_TYPE::LOAD_TYPE;
					}
					//5
					ROB.tail = (ROB.tail + 1) % Global::ROB_SIZE;

					entry_made = true;
					break;
				}
			}
		}

		//set up info for reg X
		else
		{
			if (Register_File[URF_SIZE - 1].status == Global::REGISTER_ALLOCATION::REG_UNALLOCATED)
			{
				output_struct.instruction.dest.tag = URF_SIZE - 1;

				//2
				Register_File[URF_SIZE - 1].status = Global::REGISTER_ALLOCATION::ALLOC_NO_COMMIT;
				//3
				Front_End_RAT[Global::ARCH_REGISTERS::X].reg = URF_SIZE - 1;
				Front_End_RAT[arch_dest].src_bit = Global::SOURCES::ROB;
				Front_End_RAT[arch_dest].rob_loc = ROB.tail;
				//4
				ROB.entries[ROB.tail].alloc = Global::ROB_ALLOCATION::WAITING;
				ROB.entries[ROB.tail].destReg = URF_SIZE - 1;
				ROB.entries[ROB.tail].destArchReg = Global::ARCH_REGISTERS::X;
				ROB.entries[ROB.tail].flags = Global::FLAGS::CLEAR;
				ROB.entries[ROB.tail].pc_value = myStruct.pc_value;
				ROB.entries[ROB.tail].result = 0;
				ROB.entries[ROB.tail].instruction = myStruct.untouched_instruction;

				if (output_struct.instruction.op_code != Global::OPCODE::STORE)
				{
					ROB.entries[ROB.tail].type = Global::INSTRUCTION_TYPE::BRANCH_TYPE;
				}
				else
				{
					ROB.entries[ROB.tail].type = Global::INSTRUCTION_TYPE::STORE_TYPE;
				}
				//5
				ROB.tail = (ROB.tail + 1) % Global::ROB_SIZE;

				entry_made = true;
			}
		}

		if (!entry_made)
		{
			Stalled_Stages[Global::STALLED_STAGE::DECODE_RF] = true;
			cout << endl
				<< "No available ROB entries" << endl
				<< endl;
		}

	}
	snapshot_after = output_struct;
	return output_struct;
}

void Decode::setPipelineStruct(Global::apexStruct input_struct)
{
	myStruct = input_struct;
}

bool Decode::hasValidData()
{
	return (myStruct.pc_value != INT_MAX);
}

string Decode::getInstruction()
{
	return myStruct.untouched_instruction;
}

void Decode::display()
{
	//make sure we have valid data
	if (snapshot_before.pc_value != INT_MAX)
	{
		Global::Output("Decode/Rename1  - " + snapshot_before.untouched_instruction);
	}
	else
	{
		Global::Output("Decode/Rename1 STAGE --> No Instruction in Stage");
	}
}
