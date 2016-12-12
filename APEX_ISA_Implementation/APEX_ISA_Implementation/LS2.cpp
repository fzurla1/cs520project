#include "stdafx.h"
#include "LS2.h"


LS2::LS2()
{
}


LS2::~LS2()
{
}

Global::apexStruct LS2::run(Global::Forwarding_Info(&Forward_Bus)[Global::FINAL_FORWARD_TYPE_TOTAL],
	bool(&Stalled_Stages)[Global::FINAL_STALLED_STAGE_TOTAL],
	int(&Memory_Array)[Global::MEMORY_SIZE], Global::Reorder_Buffer(&ROB))
{
	Global::apexStruct output_struct = myStruct;
	snapshot_before = myStruct;
	Stalled_Stages[Global::STALLED_STAGE::LS2] = false;

	//make sure we have valid data
	if (myStruct.pc_value != INT_MAX)
	{
		if (output_struct.instruction.op_code == Global::OPCODE::LOAD)
		{
			output_struct.instruction.dest.value = Memory_Array[output_struct.instruction.memory_location / 4];
			output_struct.instruction.dest.status = Global::STATUS::VALID;
			Forward_Bus[Global::FROM_LS2].pc_value = output_struct.pc_value;
			Forward_Bus[Global::FROM_LS2].reg_info.tag = output_struct.instruction.dest.tag;
			Forward_Bus[Global::FROM_LS2].reg_info.value = output_struct.instruction.dest.value;
			//ROB STUFF
			ROB.entries[output_struct.instruction.dest.rob_loc].alloc = Global::ROB_ALLOCATION::COMPLETE;
			ROB.entries[output_struct.instruction.dest.rob_loc].flags = output_struct.instruction.flag;
			ROB.entries[output_struct.instruction.dest.rob_loc].result = output_struct.instruction.dest.value;
			ROB.entries[output_struct.instruction.dest.rob_loc].flags = Forward_Bus[Global::FORWARD_TYPE::FROM_LS2].flag;
		}
		else if (output_struct.instruction.op_code == Global::OPCODE::STORE)
		{
			Memory_Array[output_struct.instruction.memory_location / 4] = output_struct.instruction.src1.value;
			output_struct.instruction.dest.status = Global::STATUS::VALID;
			Forward_Bus[Global::FROM_LS2].pc_value = output_struct.pc_value;
			Forward_Bus[Global::FROM_LS2].reg_info.tag = output_struct.instruction.dest.tag;
			Forward_Bus[Global::FROM_LS2].reg_info.value = output_struct.instruction.dest.value;
			//ROB STUFF
			ROB.entries[output_struct.instruction.dest.rob_loc].alloc = Global::ROB_ALLOCATION::COMPLETE;
			ROB.entries[output_struct.instruction.dest.rob_loc].flags = output_struct.instruction.flag;
			ROB.entries[output_struct.instruction.dest.rob_loc].result = output_struct.instruction.dest.value;
			ROB.entries[output_struct.instruction.dest.rob_loc].flags = Forward_Bus[Global::FORWARD_TYPE::FROM_LS2].flag;
		}
	}
	return output_struct;
}

void LS2::setPipelineStruct(Global::apexStruct input_struct)
{
	myStruct = input_struct;
}

bool LS2::hasValidData()
{
	return (myStruct.pc_value != INT_MAX);
}

void LS2::display()
{
	//make sure we have valid data
	if (myStruct.pc_value != INT_MAX)
	{
		Global::Output("LS2     - " + snapshot_after.untouched_instruction);
	}
	else
	{
		Global::Output("LS2 STAGE --> No Instruction in Stage");
	}
}

string LS2::getInstruction()
{
	return myStruct.untouched_instruction;
}