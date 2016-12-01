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
	bool(&ALU_Flags)[Global::FINAL_FLAGS_TOTAL],
	Global::Forwarding_Info(&Forward_Bus)[Global::FINAL_FORWARD_TYPE_TOTAL],
	bool(&Stalled_Stages)[Global::FINAL_STALLED_STAGE_TOTAL])
{
	Global::apexStruct output_struct = myStruct;
	snapshot_before = myStruct;

	//initialize flags
	Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].flag = Global::FLAGS::CLEAR;

	//make sure we have valid data
	if (myStruct.pc_value != INT_MAX)
	{
		if (!Stalled_Stages[Global::STALLED_STAGE::MEMORY])
		{
			//assume we are not stalled
			Stalled_Stages[Global::STALLED_STAGE::ALU2] = false;

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

#pragma region "MUL"
			case Global::OPCODE::MUL:

				output_struct.instruction.dest.value =
					myStruct.instruction.src1.value * myStruct.instruction.src2.value;

				break;
#pragma endregion

#pragma region "MUL w/ literal"
			case Global::OPCODE::MULL:

				output_struct.instruction.dest.value =
					myStruct.instruction.src1.value * myStruct.instruction.literal_value;

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
			case Global::OPCODE::LOAD:
				output_struct.instruction.memory_location =
					myStruct.instruction.src1.value + myStruct.instruction.literal_value;
				break;
			case Global::OPCODE::STORE:
				output_struct.instruction.memory_location =
					myStruct.instruction.src2.value + myStruct.instruction.literal_value;
				break;
			case Global::OPCODE::MOVC:
				output_struct.instruction.dest.value = myStruct.instruction.literal_value;
				break;
#pragma endregion

			default:
				break;
			}

			//forward data
			switch (output_struct.instruction.op_code)
			{
				//forward everything except LOAD and STORE data
			case Global::OPCODE::ADD:
			case Global::OPCODE::ADDL:
			case Global::OPCODE::SUB:
			case Global::OPCODE::SUBL:
			case Global::OPCODE::MUL:
			case Global::OPCODE::MULL:
			case Global::OPCODE::OR:
			case Global::OPCODE::ORL:
			case Global::OPCODE::AND:
			case Global::OPCODE::ANDL:
			case Global::OPCODE::EX_OR:
			case Global::OPCODE::EX_ORL:
			case Global::OPCODE::MOVC:
				output_struct.instruction.dest.status = Global::STATUS::VALID;
				Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].pc_value = output_struct.pc_value;
				Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].reg_info.tag = output_struct.instruction.dest.tag;
				Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].reg_info.value = output_struct.instruction.dest.value;

				break;

				//No need to forward anything from the LOAD or STORE instruction at this point
			case Global::OPCODE::LOAD:
			case Global::OPCODE::STORE:
			default:
				break;
			}

			if (output_struct.instruction.dest.value == 0)
			{
				ALU_Flags[Global::FLAGS::ZERO] = true;
				Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].flag = Global::FLAGS::ZERO;
			}
			else
			{
				ALU_Flags[Global::FLAGS::ZERO] = false;
			}

			if (!ALU_Flags[Global::FLAGS::ZERO])
			{
				ALU_Flags[Global::FLAGS::CLEAR] = true;
				Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].flag = Global::FLAGS::CLEAR;
			}
			else
			{
				ALU_Flags[Global::FLAGS::CLEAR] = false;
			}
		}
		else
		{
			Stalled_Stages[Global::STALLED_STAGE::ALU2] = true;
		}
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
		Global::Debug("ALU2       - " + snapshot_before.untouched_instruction);
	}
	else
	{
		Global::Debug("ALU 2 STAGE --> No Instruction in Stage");
	}
}