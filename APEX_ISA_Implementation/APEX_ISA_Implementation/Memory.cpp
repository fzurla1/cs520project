#include "stdafx.h"
#include "Memory.h"

using namespace std;
Memory::Memory()
{
}


Memory::~Memory()
{
}

//at this point, memory location must already be computed, therefore, src1 value must be known.
//this mean that we could potentially still require the 1st operand for a store command
Global::apexStruct Memory::run(
	Global::Forwarding_Info(&Forward_Bus)[Global::FINAL_FORWARD_TYPE_TOTAL],
	bool (&Stalled_Stages)[Global::FINAL_STALLED_STAGE_TOTAL],
	int (&Memory_Array)[Global::MEMORY_SIZE])
{
	Global::apexStruct output_struct = myStruct;
	snapshot_before = myStruct;
	Stalled_Stages[Global::STALLED_STAGE::MEMORY] = false;

	//make sure we have valid data
	if (myStruct.pc_value != INT_MAX)
	{
		if (output_struct.instruction.op_code == Global::OPCODE::LOAD)
		{
			output_struct.instruction.dest.value = Memory_Array[myStruct.instruction.memory_location/4];
			output_struct.instruction.dest.status = Global::STATUS::VALID;
			Forward_Bus[Global::FROM_MEMORY].pc_value = output_struct.pc_value;
			Forward_Bus[Global::FROM_MEMORY].reg_info.tag = output_struct.instruction.dest.tag;
			Forward_Bus[Global::FROM_MEMORY].reg_info.value = output_struct.instruction.dest.value;
		}
		else if (output_struct.instruction.op_code == Global::OPCODE::STORE)
		{
			if (myStruct.instruction.src1.status == Global::STATUS::INVALID)
			{
				if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_info.tag == myStruct.instruction.src1.tag)
				{
					output_struct.instruction.src1.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_info.value;
					output_struct.instruction.src1.status = Global::STATUS::VALID;
					Memory_Array[myStruct.instruction.memory_location/4] = output_struct.instruction.src1.value;

					//write bogus info into forward bus
					Forward_Bus[Global::FROM_MEMORY].pc_value = -1;
					Forward_Bus[Global::FROM_MEMORY].reg_info.tag = -1;
					Forward_Bus[Global::FROM_MEMORY].reg_info.value = -1;
				}
				else
				{
					Stalled_Stages[Global::STALLED_STAGE::MEMORY] = true;
				}
			}
			else
			{
				Memory_Array[myStruct.instruction.memory_location] = output_struct.instruction.src1.value;

				//write bogus info into forward bus
				Forward_Bus[Global::FROM_MEMORY].pc_value = -1;
				Forward_Bus[Global::FROM_MEMORY].reg_info.tag = -1;
				Forward_Bus[Global::FROM_MEMORY].reg_info.value = -1;
			}
		}
	}

	//save data for viewing
	snapshot_after = output_struct;

	return output_struct;
}

void Memory::setPipelineStruct(Global::apexStruct input_struct)
{
	myStruct = input_struct;
}

bool Memory::hasValidData()
{
	return (myStruct.pc_value != INT_MAX);
}

string Memory::getInstruction()
{
	return myStruct.untouched_instruction;
}

void Memory::display()
{
	//make sure we have valid data
	if (myStruct.pc_value != INT_MAX)
	{
		Global::Output("MEMORY     - " + snapshot_after.untouched_instruction);
		/*
		Global::Output("--- MEM stage display ---");
		Global::Output(" - Entering stage - ");
		Global::Output("pc                  : " + to_string(4000 + ((snapshot_before.pc_value - 4000) * 4)));
		Global::Output("raw instruction     : " + snapshot_before.untouched_instruction);
		Global::Output("op code             : " + Global::toString(snapshot_before.instruction.op_code));
		Global::Output("destination reg tag : " + Global::toString(snapshot_before.instruction.dest.tag));
		Global::Output("destination value   : N/A");
		Global::Output("source 1 reg tag    : " + Global::toString(snapshot_before.instruction.src1.tag));
		Global::Output("source 1 reg valid  : " + Global::toString(snapshot_before.instruction.src1.status));
		Global::Output("source 1 reg value  : " + to_string(snapshot_before.instruction.src1.value));
		Global::Output("source 2 reg tag    : " + Global::toString(snapshot_before.instruction.src2.tag));
		Global::Output("source 2 reg valid  : " + Global::toString(snapshot_before.instruction.src2.status));
		Global::Output("source 2 reg value  : " + to_string(snapshot_before.instruction.src2.value));
		Global::Output("literal             : " + to_string(snapshot_before.instruction.literal_value));
		Global::Output("....................");
		Global::Output(" - Exiting stage - ");
		Global::Output("pc                  : " + to_string(4000 + ((snapshot_after.pc_value - 4000) * 4)));
		Global::Output("op code             : " + Global::toString(snapshot_after.instruction.op_code));
		Global::Output("destination reg tag : " + Global::toString(snapshot_after.instruction.dest.tag));
		Global::Output("destination value   : " + to_string(snapshot_after.instruction.dest.value));
		Global::Output("source 1 reg tag    : " + Global::toString(snapshot_after.instruction.src1.tag));
		Global::Output("source 1 reg valid  : " + Global::toString(snapshot_after.instruction.src1.status));
		Global::Output("source 1 reg value  : " + to_string(snapshot_after.instruction.src1.value));
		Global::Output("source 2 reg tag    : " + Global::toString(snapshot_after.instruction.src2.tag));
		Global::Output("source 2 reg valid  : " + Global::toString(snapshot_after.instruction.src2.status));
		Global::Output("source 2 reg value  : " + to_string(snapshot_after.instruction.src2.value));
		Global::Output("literal             : " + to_string(snapshot_after.instruction.literal_value));
		Global::Output("memory location     : " + to_string(snapshot_after.instruction.memory_location));
		Global::Output("--- END MEM stage display ---");
		*/
	}
	else
	{
		Global::Output("MEM STAGE --> No Instruction in Stage");
	}
}