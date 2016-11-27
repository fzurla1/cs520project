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
	Global::Register_Info(&Register_File)[Global::REGISTERS::FINAL_REGISTERS_TOTAL],
	Global::Forwarding_Info(&Forward_Bus)[Global::FORWARD_TYPE::FINAL_FORWARD_TYPE_TOTAL],
	bool(&Stalled_Stages)[Global::STALLED_STAGE::FINAL_STALLED_STAGE_TOTAL],
	//int(&Most_Recent_Reg)[Global::REGISTERS::FINAL_REGISTERS_TOTAL],
	Global::Reorder_Buffer(&ROB),
	Global::Rename_Table(&Front_End_RAT))
{
	Global::apexStruct output_struct = myStruct;
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

				last_alu_pc = output_struct.pc_value;

				arch_dest = atoi(s_dest.c_str());
				arch_src1 = atoi(s_reg1.c_str());
				arch_src2 = atoi(s_reg2.c_str());

				//set up source 1
				output_struct.instruction.src1.status = Global::STATUS::VALID;
				output_struct.instruction.src1.tag = Front_End_RAT.reg[arch_src1];
				output_struct.instruction.src1.rob_loc = -1;

				//set up source 2
				output_struct.instruction.src2.status = Global::STATUS::VALID;
				output_struct.instruction.src2.tag = Front_End_RAT.reg[arch_src2];
				output_struct.instruction.src2.rob_loc = -1;

				//FOR SRC1
				//look to FE RAT for where data is stored
				if ((Front_End_RAT.src_bit[arch_src1] == Global::SOURCES::REGISTER_FILE)
					&& (Register_File[Front_End_RAT.reg[arch_src1]].status == Global::REGISTER_ALLOCATION::ALLOC_COMMIT))
				{
					output_struct.instruction.src1.value = Register_File[output_struct.instruction.src1.tag].value;
				}

				//check ROB and forwarding buses
				//if tag matches what we are looking for, and that instruction is the most recent to update 
				//the register, then grab that value, otherwise, stall.
				else
				{
					//get ROB entry location
					output_struct.instruction.src1.rob_loc = Front_End_RAT.reg[arch_src1];

					//check ROB
					//if ROB entry is complete, get out data
					//get ROB entry # from FE RAT ROB location entry based on arch register from src1 input
					if (ROB.entries[Front_End_RAT.rob_loc[arch_src1]].alloc == Global::ROB_ALLOCATION::COMPLETE)
					{
						output_struct.instruction.src1.value = ROB.entries[Front_End_RAT.rob_loc[arch_src1]].result;
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
						else if (Forward_Bus[Global::FORWARD_TYPE::FROM_MEMORY].reg_info.tag == output_struct.instruction.src1.tag)
						{
							output_struct.instruction.src1.value = Forward_Bus[Global::FORWARD_TYPE::FROM_MEMORY].reg_info.value;
						}
						//check from writeback
						else if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_info.tag == output_struct.instruction.src1.tag)
						{
							output_struct.instruction.src1.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_info.value;
						}
						else
						{
							output_struct.instruction.src1.status = Global::STATUS::INVALID;
						}
					}
				}

				//src2
				//look to reg file for valid data
				//FOR SRC1
				//look to FE RAT for where data is stored
				if ((Front_End_RAT.src_bit[arch_src2] == Global::SOURCES::REGISTER_FILE)
					&& (Register_File[Front_End_RAT.reg[arch_src2]].status == Global::REGISTER_ALLOCATION::ALLOC_COMMIT))
				{
					output_struct.instruction.src2.value = Register_File[output_struct.instruction.src2.tag].value;
				}

				//check ROB and forwarding buses
				//if tag matches what we are looking for, and that instruction is the most recent to update 
				//the register, then grab that value, otherwise, stall.
				else
				{
					//get ROB entry location
					output_struct.instruction.src2.rob_loc = Front_End_RAT.reg[arch_src2];

					//check ROB
					//if ROB entry is complete, get out data
					//get ROB entry # from FE RAT ROB location entry based on arch register from src1 input
					if (ROB.entries[Front_End_RAT.rob_loc[arch_src2]].alloc == Global::ROB_ALLOCATION::COMPLETE)
					{
						output_struct.instruction.src2.value = ROB.entries[Front_End_RAT.rob_loc[arch_src2]].result;
						output_struct.instruction.src2.status = Global::STATUS::VALID;
					}
					//if the entry is not complete, but it is executing or waiting, check forwarding bus
					else
					{
						//check from ALU2
						if (Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].reg_info.tag == output_struct.instruction.src2.tag)
						{
							output_struct.instruction.src2.value = Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].reg_info.value;
						}
						//check from memory
						else if (Forward_Bus[Global::FORWARD_TYPE::FROM_MEMORY].reg_info.tag == output_struct.instruction.src2.tag)
						{
							output_struct.instruction.src2.value = Forward_Bus[Global::FORWARD_TYPE::FROM_MEMORY].reg_info.value;
						}
						//check from writeback
						else if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_info.tag == output_struct.instruction.src2.tag)
						{
							output_struct.instruction.src2.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_info.value;
						}
						else
						{
							output_struct.instruction.src2.status = Global::STATUS::INVALID;
						}
					}
				}

				//set up destination
				//1) find a register that is unallocated
				//2) mark as allocated, but not committed
				//3) update FE RAT with register and mark source bit as ROB
				//4) create new entry in ROB
				//5) move tail
				for (int x = 0; x < Global::REGISTERS::FINAL_REGISTERS_TOTAL; x++)
				{
					//1
					if (Register_File[x].status == Global::REGISTER_ALLOCATION::REG_UNALLOCATED)
					{
						//2
						Register_File[x].status = Global::REGISTER_ALLOCATION::ALLOC_NO_COMMIT;
						//3
						Front_End_RAT.reg[arch_dest] = Global::REGISTERS(x);
						Front_End_RAT.src_bit[arch_dest] = Global::SOURCES::ROB;
						Front_End_RAT.rob_loc[arch_dest] = ROB.tail;
						//4
						ROB.entries[ROB.tail].alloc = Global::ROB_ALLOCATION::WAITING;
						ROB.entries[ROB.tail].destReg = Global::REGISTERS(x);
						ROB.entries[ROB.tail].flags = Global::FLAGS::CLEAR;
						ROB.entries[ROB.tail].pc_value = myStruct.pc_value;
						ROB.entries[ROB.tail].result = 0;
						ROB.entries[ROB.tail].type = Global::INSTRUCTION_TYPE::REG_TO_REG_TYPE;
						//5
						ROB.tail = (ROB.tail + 1) % Global::ROB_SIZE;
						break;
					}
				}
				destination = ALU;
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

				last_alu_pc = output_struct.pc_value;

				arch_dest = atoi(s_dest.c_str());
				arch_src1 = atoi(s_reg1.c_str());

				//set up source 1
				output_struct.instruction.src1.status = Global::STATUS::VALID;
				output_struct.instruction.src1.tag = Front_End_RAT.reg[arch_src1];
				output_struct.instruction.src1.rob_loc = -1;

				//set source 2 to valid since there isnt one
				output_struct.instruction.src2.status = Global::STATUS::VALID;

				//set up literal
				output_struct.instruction.literal_value = atoi(s_literal.c_str());

				//FOR SRC1
				//look to FE RAT for where data is stored
				if ((Front_End_RAT.src_bit[arch_src1] == Global::SOURCES::REGISTER_FILE)
					&& (Register_File[Front_End_RAT.reg[arch_src1]].status == Global::REGISTER_ALLOCATION::ALLOC_COMMIT))
				{
					output_struct.instruction.src1.value = Register_File[output_struct.instruction.src1.tag].value;
				}

				//check ROB and forwarding buses
				//if tag matches what we are looking for, and that instruction is the most recent to update 
				//the register, then grab that value, otherwise, stall.
				else
				{
					//get ROB entry location
					output_struct.instruction.src1.rob_loc = Front_End_RAT.reg[arch_src1];

					//check ROB
					//if ROB entry is complete, get out data
					//get ROB entry # from FE RAT ROB location entry based on arch register from src1 input
					if (ROB.entries[Front_End_RAT.rob_loc[arch_src1]].alloc == Global::ROB_ALLOCATION::COMPLETE)
					{
						output_struct.instruction.src1.value = ROB.entries[Front_End_RAT.rob_loc[arch_src1]].result;
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
						else if (Forward_Bus[Global::FORWARD_TYPE::FROM_MEMORY].reg_info.tag == output_struct.instruction.src1.tag)
						{
							output_struct.instruction.src1.value = Forward_Bus[Global::FORWARD_TYPE::FROM_MEMORY].reg_info.value;
						}
						//check from writeback
						else if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_info.tag == output_struct.instruction.src1.tag)
						{
							output_struct.instruction.src1.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_info.value;
						}
						else
						{
							output_struct.instruction.src1.status = Global::STATUS::INVALID;
						}
					}
				}

				//set up destination
				//1) find a register that is unallocated
				//2) mark as allocated, but not committed
				//3) update FE RAT with register and mark source bit as ROB
				//4) create new entry in ROB
				//5) move tail
				for (int x = 0; x < Global::REGISTERS::FINAL_REGISTERS_TOTAL; x++)
				{
					//1
					if (Register_File[x].status == Global::REGISTER_ALLOCATION::REG_UNALLOCATED)
					{
						//2
						Register_File[x].status = Global::REGISTER_ALLOCATION::ALLOC_NO_COMMIT;
						//3
						Front_End_RAT.reg[arch_dest] = Global::REGISTERS(x);
						Front_End_RAT.src_bit[arch_dest] = Global::SOURCES::ROB;
						Front_End_RAT.rob_loc[arch_dest] = ROB.tail;
						//4
						ROB.entries[ROB.tail].alloc = Global::ROB_ALLOCATION::WAITING;
						ROB.entries[ROB.tail].destReg = Global::REGISTERS(x);
						ROB.entries[ROB.tail].flags = Global::FLAGS::CLEAR;
						ROB.entries[ROB.tail].pc_value = myStruct.pc_value;
						ROB.entries[ROB.tail].result = 0;
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
						break;
					}
				}

				destination = ALU;
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
				output_struct.instruction.src1.tag = Front_End_RAT.reg[arch_src1];
				output_struct.instruction.src1.rob_loc = -1;

				//set up source 2
				output_struct.instruction.src2.status = Global::STATUS::VALID;
				output_struct.instruction.src2.tag = Front_End_RAT.reg[arch_src2];
				output_struct.instruction.src2.rob_loc = -1;

				output_struct.instruction.literal_value = atoi(s_literal.c_str());

				//FOR SRC1
				//look to FE RAT for where data is stored
				if ((Front_End_RAT.src_bit[arch_src1] == Global::SOURCES::REGISTER_FILE)
					&& (Register_File[Front_End_RAT.reg[arch_src1]].status == Global::REGISTER_ALLOCATION::ALLOC_COMMIT))
				{
					output_struct.instruction.src1.value = Register_File[output_struct.instruction.src1.tag].value;
				}

				//check ROB and forwarding buses
				//if tag matches what we are looking for, and that instruction is the most recent to update 
				//the register, then grab that value, otherwise, stall.
				else
				{
					//get ROB entry location
					output_struct.instruction.src1.rob_loc = Front_End_RAT.reg[arch_src1];

					//check ROB
					//if ROB entry is complete, get out data
					//get ROB entry # from FE RAT ROB location entry based on arch register from src1 input
					if (ROB.entries[Front_End_RAT.rob_loc[arch_src1]].alloc == Global::ROB_ALLOCATION::COMPLETE)
					{
						output_struct.instruction.src1.value = ROB.entries[Front_End_RAT.rob_loc[arch_src1]].result;
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
						else if (Forward_Bus[Global::FORWARD_TYPE::FROM_MEMORY].reg_info.tag == output_struct.instruction.src1.tag)
						{
							output_struct.instruction.src1.value = Forward_Bus[Global::FORWARD_TYPE::FROM_MEMORY].reg_info.value;
						}
						//check from writeback
						else if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_info.tag == output_struct.instruction.src1.tag)
						{
							output_struct.instruction.src1.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_info.value;
						}
						else
						{
							output_struct.instruction.src1.status = Global::STATUS::INVALID;
						}
					}
				}

				//src2
				//look to reg file for valid data
				//FOR SRC1
				//look to FE RAT for where data is stored
				if ((Front_End_RAT.src_bit[arch_src2] == Global::SOURCES::REGISTER_FILE)
					&& (Register_File[Front_End_RAT.reg[arch_src2]].status == Global::REGISTER_ALLOCATION::ALLOC_COMMIT))
				{
					output_struct.instruction.src2.value = Register_File[output_struct.instruction.src2.tag].value;
				}

				//check ROB and forwarding buses
				//if tag matches what we are looking for, and that instruction is the most recent to update 
				//the register, then grab that value, otherwise, stall.
				else
				{
					//get ROB entry location
					output_struct.instruction.src2.rob_loc = Front_End_RAT.reg[arch_src2];

					//check ROB
					//if ROB entry is complete, get out data
					//get ROB entry # from FE RAT ROB location entry based on arch register from src1 input
					if (ROB.entries[Front_End_RAT.rob_loc[arch_src2]].alloc == Global::ROB_ALLOCATION::COMPLETE)
					{
						output_struct.instruction.src2.value = ROB.entries[Front_End_RAT.rob_loc[arch_src2]].result;
						output_struct.instruction.src2.status = Global::STATUS::VALID;
					}
					//if the entry is not complete, but it is executing or waiting, check forwarding bus
					else
					{
						//check from ALU2
						if (Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].reg_info.tag == output_struct.instruction.src2.tag)
						{
							output_struct.instruction.src2.value = Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].reg_info.value;
						}
						//check from memory
						else if (Forward_Bus[Global::FORWARD_TYPE::FROM_MEMORY].reg_info.tag == output_struct.instruction.src2.tag)
						{
							output_struct.instruction.src2.value = Forward_Bus[Global::FORWARD_TYPE::FROM_MEMORY].reg_info.value;
						}
						//check from writeback
						else if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_info.tag == output_struct.instruction.src2.tag)
						{
							output_struct.instruction.src2.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_info.value;
						}
						else
						{
							output_struct.instruction.src2.status = Global::STATUS::INVALID;
						}
					}
				}

				destination = ALU;
				break;

			case Global::OPCODE::BNZ:
			case Global::OPCODE::BZ:
				iss >> s_literal;
				s_literal = s_literal.substr(1, s_literal.length());
				output_struct.instruction.literal_value = atoi(s_literal.c_str());

				output_struct.instruction.dest.status = Global::STATUS::VALID;
				output_struct.instruction.src1.status = Global::STATUS::VALID;
				output_struct.instruction.src2.status = Global::STATUS::VALID;
				output_struct.branch_waiting_to_complete = last_alu_pc;
				break;
				//opcode <src1>, #literal
			case Global::OPCODE::BAL:
			case Global::OPCODE::MOVC:
				iss >> s_dest;
				s_dest = s_dest.substr(1, (s_dest.length() - 1));
				iss >> s_literal;
				s_literal = s_literal.substr(1, s_literal.length());

				output_struct.instruction.dest.tag = Global::REGISTERS(atoi(s_dest.c_str()));

				output_struct.instruction.src1.tag = Global::REGISTERS::UNA;
				output_struct.instruction.src1.status = Global::STATUS::VALID;

				output_struct.instruction.src2.tag = Global::REGISTERS::UNA;
				output_struct.instruction.src2.status = Global::STATUS::VALID;

				output_struct.instruction.literal_value = atoi(s_literal.c_str());

				destination = BRANCH;
				break;
			case Global::OPCODE::JUMP:
				iss >> s_dest;
				s_dest = s_dest.substr(1, (s_dest.length() - 1));
				iss >> s_literal;
				s_literal = s_literal.substr(1, s_literal.length());

				output_struct.instruction.dest.tag = Global::ARCH_REGISTERS::NA;

				output_struct.instruction.src1.tag = Global::ARCH_REGISTERS::X;
				output_struct.instruction.src1.status = Global::STATUS::VALID;
				output_struct.instruction.src1.value = Register_File[Global::ARCH_REGISTERS::X].value;

				output_struct.instruction.src2.tag = Global::ARCH_REGISTERS::NA;
				output_struct.instruction.src2.status = Global::STATUS::VALID;

				output_struct.instruction.literal_value = atoi(s_literal.c_str());

				destination = BRANCH;
			default:
				break;

		}

		if (output_struct.instruction.op_code == Global::OPCODE::MOVC)
		{
			destination = ALU;
		}

		//set up the destination information within the register file and most recently used array
		output_struct.instruction.dest.status = Global::STATUS::INVALID;

		if ((output_struct.instruction.src1.status == Global::STATUS::VALID) 
			&& (output_struct.instruction.src2.status == Global::STATUS::VALID))
		{
			Register_File[output_struct.instruction.dest.tag].status = output_struct.instruction.dest.status;
			if (output_struct.instruction.dest.tag != Global::ARCH_REGISTERS::NA)
			{
				Most_Recent_Reg[output_struct.instruction.dest.tag] = output_struct.pc_value;
			}
		}

		//figure out if we should stall based on OPCODE
		//if ALU1 is stalled, and the next instruction is for ALU1, stall
		if ((destination == ALU)
			&& (Stalled_Stages[Global::STALLED_STAGE::ALU1] == true))
		{
			Stalled_Stages[Global::STALLED_STAGE::DECODE_RF] = true;
		}

		//else if Branch is stalled, and the next instruction is for Branch, stall
		else if ((destination == BRANCH)
			&& (Stalled_Stages[Global::STALLED_STAGE::BRANCH] == true))
		{
			Stalled_Stages[Global::STALLED_STAGE::DECODE_RF] = true;
		}

		if (Stalled_Stages[Global::STALLED_STAGE::DECODE_RF] == true)
		{
			myStruct = output_struct;
		}

		//look for values from forward bus first, then from register file

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
		Global::Debug("DECODE/RF  - " + snapshot_before.untouched_instruction);
		/*
		Global::Debug("\n--- Decode/RF stage display ---\n - ENTERING STAGE -");
		Global::Debug("pc                  : " + to_string(4000 + ((snapshot_before.pc_value - 4000) * 4)));
		Global::Debug("raw instruction     : " + snapshot_before.untouched_instruction);
		Global::Debug("op code             : " + Global::toString(snapshot_before.instruction.op_code));
		Global::Debug("destination reg tag : " + Global::toString(snapshot_before.instruction.dest.tag));
		Global::Debug("destination value   : not ready");
		Global::Debug("source 1 reg tag    : " + Global::toString(snapshot_before.instruction.src1.tag));
		Global::Debug("source 1 reg valid  : " + Global::toString(snapshot_before.instruction.src1.status));

		if (snapshot_before.instruction.src1.status == Global::STATUS::INVALID)
			Global::Debug("source 1 reg value  : invalid!");
		else
			Global::Debug("source 1 reg value  : " + to_string(snapshot_before.instruction.src1.value));

		Global::Debug("source 2 reg tag    : " + Global::toString(snapshot_before.instruction.src2.tag));
		Global::Debug("source 2 reg valid  : " + Global::toString(snapshot_before.instruction.src2.status));

		if (!snapshot_before.instruction.src2.status)
			Global::Debug("source 2 reg value  : invalid!");
		else
			Global::Debug("source 2 reg value  : " + to_string(snapshot_before.instruction.src2.value));

		Global::Debug("literal             : " + to_string(snapshot_before.instruction.literal_value));
		Global::Debug(".....................");

		Global::Debug(" - EXITING STAGE -");
		Global::Debug("pc                  : " + to_string(4000 + ((snapshot_after.pc_value - 4000) * 4)));
		Global::Debug("op code             : " + Global::toString(snapshot_after.instruction.op_code));
		Global::Debug("destination reg tag : " + Global::toString(snapshot_after.instruction.dest.tag));
		Global::Debug("destination value   : not ready");
		Global::Debug("source 1 reg tag    : " + Global::toString(snapshot_after.instruction.src1.tag));
		Global::Debug("source 1 reg valid  : " + Global::toString(snapshot_after.instruction.src1.status));

		if (!snapshot_after.instruction.src1.status)
			Global::Debug("source 1 reg value  : invalid!");
		else
			Global::Debug("source 1 reg value  : " + to_string(snapshot_after.instruction.src1.value));

		Global::Debug("source 2 reg tag    : " + Global::toString(snapshot_after.instruction.src2.tag));
		Global::Debug("source 2 reg valid  : " + Global::toString(snapshot_after.instruction.src2.status));

		if (!snapshot_after.instruction.src2.status)
			Global::Debug("source 2 reg value  : invalid!");
		else
			Global::Debug("source 2 reg value  : " + to_string(snapshot_after.instruction.src2.value));

		Global::Debug("literal             : " + to_string(snapshot_after.instruction.literal_value));

		Global::Debug("--- END Decode/RF stage display ---");
		*/
	}
	else
	{
		Global::Debug("Decode/RF STAGE --> No Instruction in Stage");
	}
}
