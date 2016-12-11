#include "stdafx.h"
#include "ALU2.h"

using namespace std;

extern int Stalled_Stages;
extern bool ALU_Flags;
extern Global::Register_Info Forward_Bus;

ALU2::ALU2()
{
}


ALU2::~ALU2()
{
}

Global::apexStruct ALU2::run(
	Global::Forwarding_Info(&Forward_Bus)[Global::FINAL_FORWARD_TYPE_TOTAL],
	bool(&Stalled_Stages)[Global::FINAL_STALLED_STAGE_TOTAL],
	Global::Reorder_Buffer(&ROB))
{
	Global::apexStruct output_struct = myStruct;
	snapshot_before = myStruct;

	//initialize flags
	Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].flag = Global::FLAGS::CLEAR;

	//make sure we have valid data
	if (myStruct.pc_value != INT_MAX)
	{
		switch (myStruct.instruction.op_code)
		{
#pragma region "ADD"
			case Global::OPCODE::ADD:
				output_struct.instruction.dest.value =
					myStruct.instruction.src1.value + myStruct.instruction.src2.value;

				break;
#pragma endregion

#pragma region "ADD w/ literal"
			case Global::OPCODE::ADDL:
				output_struct.instruction.dest.value =
					myStruct.instruction.src1.value + myStruct.instruction.literal_value;

				break;
#pragma endregion

#pragma region "SUB"
			case Global::OPCODE::SUB:
				output_struct.instruction.dest.value =
					myStruct.instruction.src1.value - myStruct.instruction.src2.value;

				break;
#pragma endregion

#pragma region "SUB w/ lieral"
			case Global::OPCODE::SUBL:

				output_struct.instruction.dest.value =
					myStruct.instruction.src1.value - myStruct.instruction.literal_value;

				break;
#pragma endregion 

#pragma region"ORs"
			case Global::OPCODE::OR:
				output_struct.instruction.dest.value =
					myStruct.instruction.src1.value | myStruct.instruction.src2.value;
				break;
			case Global::OPCODE::ORL:
				output_struct.instruction.dest.value =
					myStruct.instruction.src1.value | myStruct.instruction.literal_value;
				break;
#pragma endregion

#pragma region "ANDs"
			case Global::OPCODE::AND:
				output_struct.instruction.dest.value =
					myStruct.instruction.src1.value & myStruct.instruction.src2.value;
				break;
			case Global::OPCODE::ANDL:
				output_struct.instruction.dest.value =
					myStruct.instruction.src1.value & myStruct.instruction.literal_value;
				break;
#pragma endregion

#pragma region "XORs"
			case Global::OPCODE::EX_OR:
				output_struct.instruction.dest.value =
					myStruct.instruction.src1.value ^ myStruct.instruction.src2.value;
				break;
			case Global::OPCODE::EX_ORL:
				output_struct.instruction.dest.value =
					myStruct.instruction.src1.value ^ myStruct.instruction.literal_value;
				break;
#pragma endregion

#pragma region "OTHERS"
			case Global::OPCODE::MOVC:
				output_struct.instruction.dest.value = myStruct.instruction.literal_value;
				break;
#pragma endregion

			default:
				break;
		}

		output_struct.instruction.dest.status = Global::STATUS::VALID;
		Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].pc_value = output_struct.pc_value;
		Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].reg_info.tag = output_struct.instruction.dest.tag;
		Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].reg_info.value = output_struct.instruction.dest.value;

		//Update ROB
		ROB.entries[output_struct.instruction.dest.rob_loc].alloc = Global::ROB_ALLOCATION::COMPLETE;
		ROB.entries[output_struct.instruction.dest.rob_loc].result = output_struct.instruction.dest.value;


		if (output_struct.instruction.dest.value == 0)
		{
			Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].flag = Global::FLAGS::ZERO;
		}
		else
		{
			Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].flag = Global::FLAGS::CLEAR;
		}

		ROB.entries[output_struct.instruction.dest.rob_loc].flags = Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].flag;
	}

	snapshot_after = output_struct;
	return output_struct;
}

void ALU2::setPipelineStruct(Global::apexStruct input_struct)
{
	myStruct = input_struct;
}

bool ALU2::hasValidData()
{
	return (myStruct.pc_value != INT_MAX);
}

string ALU2::getInstruction()
{
	return myStruct.untouched_instruction;
}

void ALU2::display()
{
	//make sure we have valid data
	if (myStruct.pc_value != INT_MAX)
	{
		Global::Output("ALU2       - " + snapshot_before.untouched_instruction);
	}
	else
	{
		Global::Output("ALU 2 STAGE --> No Instruction in Stage");
	}
}