#include "stdafx.h"
#include "LS1.h"


LS1::LS1()
{
}


LS1::~LS1()
{
}


void LS1::setPipelineStruct(Global::apexStruct input_struct)
{
	myStruct = input_struct;
}

bool LS1::hasValidData()
{
	return (myStruct.pc_value != INT_MAX);
}

Global::apexStruct LS1::run(Global::Forwarding_Info(&Forward_Bus)[Global::FINAL_FORWARD_TYPE_TOTAL],
	bool(&Stalled_Stages)[Global::FINAL_STALLED_STAGE_TOTAL],
	int(&Memory_Array)[Global::MEMORY_SIZE])
{
	Global::apexStruct output_struct = myStruct;
	snapshot_before = myStruct;
	if (Stalled_Stages[Global::STALLED_STAGE::LS2] == false)
	{
		if (myStruct.pc_value != INT_MAX)
		{
			//Get memory location
			if (output_struct.instruction.op_code == Global::OPCODE::LOAD)
			{
				output_struct.instruction.memory_location = output_struct.instruction.src1.value + output_struct.instruction.literal_value;
			}
			else if (output_struct.instruction.op_code == Global::OPCODE::STORE)
			{
				output_struct.instruction.memory_location = output_struct.instruction.src2.value + output_struct.instruction.literal_value;
			}
		}
		Stalled_Stages[Global::STALLED_STAGE::LS2] = true;
	}
	else
	{
		Stalled_Stages[Global::STALLED_STAGE::LS1] = true;
	}

	snapshot_after = output_struct;

	return output_struct;
}

void LS1::display()
{
	//make sure we have valid data
	if (myStruct.pc_value != INT_MAX)
	{
		Global::Output("LS1     - " + snapshot_after.untouched_instruction);
	}
	else
	{
		Global::Output("LS1 STAGE --> No Instruction in Stage");
	}
}