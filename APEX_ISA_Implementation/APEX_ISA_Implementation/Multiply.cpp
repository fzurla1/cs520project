#include "stdafx.h"
#include "Multiply.h"


Multiply::Multiply()
{
}


Multiply::~Multiply()
{
}


Global::apexStruct Multiply::run(Global::Forwarding_Info(&Forward_Bus)[Global::FINAL_FORWARD_TYPE_TOTAL],
	Global::Reorder_Buffer(&ROB),
	bool(&Stalled_Stages)[Global::FINAL_STALLED_STAGE_TOTAL])
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
	
	//make sure we have valid data
	if (myStruct.pc_value != INT_MAX)
	{
		//mark FU as busy
		Stalled_Stages[Global::STALLED_STAGE::MULTIPLY] = true;

		//takes 4 cycles to complete
		if (count == 4)
		{
			//mark FU as free
			Stalled_Stages[Global::STALLED_STAGE::MULTIPLY] = false;

			switch (myStruct.instruction.op_code)
			{
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
				default:
					break;
			}

			//mark as valid
			output_struct.instruction.dest.status = Global::STATUS::VALID;

			//update Forward Bus
			Forward_Bus[Global::FORWARD_TYPE::FROM_MULTIPLY].pc_value = output_struct.pc_value;
			Forward_Bus[Global::FORWARD_TYPE::FROM_MULTIPLY].reg_info.tag = output_struct.instruction.dest.tag;
			Forward_Bus[Global::FORWARD_TYPE::FROM_MULTIPLY].reg_info.value = output_struct.instruction.dest.value;

			//reset count
			count = 1;
		}
		else
		{
			count++;
		}
	}

	//Update ROB
	ROB.entries[output_struct.instruction.dest.rob_loc].alloc = Global::ROB_ALLOCATION::COMPLETE;
	ROB.entries[output_struct.instruction.dest.rob_loc].flags = output_struct.instruction.flag;
	ROB.entries[output_struct.instruction.dest.rob_loc].result = output_struct.instruction.dest.value;

	return output_struct;
}


void Multiply::setPipelineStruct(Global::apexStruct input_struct)
{
	myStruct = input_struct;
}


bool Multiply::hasValidData()
{
	return (myStruct.pc_value != INT_MAX);
}


string Multiply::getInstruction()
{
	return myStruct.untouched_instruction;
}


void Multiply::display()
{
	//make sure we have valid data
	if (myStruct.pc_value != INT_MAX)
	{
		Global::Output("MULTIPLY  - " + snapshot_before.untouched_instruction);
	}
	else
	{
		Global::Output("MULTIPLY 2 STAGE --> No Instruction in Stage");
	}
}