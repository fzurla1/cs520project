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
	Global::Forwarding_Info(&Forward_Bus)[Global::FORWARDING_BUSES],
	bool (&Stalled_Stages)[Global::TOTAL_STAGES],
	int (&Memory_Array)[Global::MEMORY_SIZE])
{
	Global::apexStruct output_struct = myStruct;
	snapshot_before = myStruct;

	//make sure we have valid data
	if (myStruct.pc_value != INT_MAX)
	{
		//get forwarded info, if needed
		if (myStruct.instruction.dest.status == Global::STATUS::INVALID)
		{
			if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_info.tag == myStruct.instruction.dest.tag)
			{
				output_struct.instruction.dest.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_info.value;
				output_struct.instruction.dest.status = Global::STATUS::VALID;
			}
			else
			{
				Stalled_Stages[Global::STALLED_STAGE::MEMORY] = true;
			}
		}
		else
		{
			if (output_struct.instruction.op_code == Global::OPCODE::LOAD)
			{
				output_struct.instruction.dest.value = Memory_Array[myStruct.instruction.memory_location];
				output_struct.instruction.dest.status = Global::STATUS::VALID;
				Forward_Bus[Global::FROM_MEMORY].pc_value = output_struct.pc_value;
				Forward_Bus[Global::FROM_MEMORY].reg_info.tag = output_struct.instruction.dest.tag;
				Forward_Bus[Global::FROM_MEMORY].reg_info.value = output_struct.instruction.dest.value;
			}
			else if (output_struct.instruction.op_code == Global::OPCODE::STORE)
			{
				Memory_Array[myStruct.instruction.memory_location] = output_struct.instruction.dest.value;

				//write bogus info into forward bus
				Forward_Bus[Global::FROM_MEMORY].pc_value = -1;
				Forward_Bus[Global::FROM_MEMORY].reg_info.tag = Global::ARCH_REGISTERS::NA;
				Forward_Bus[Global::FROM_MEMORY].reg_info.value = -1;
			}

			Stalled_Stages[Global::STALLED_STAGE::MEMORY] = false;
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

void Memory::display()
{
	//make sure we have valid data
	if (myStruct.pc_value != INT_MAX)
	{
		Global::Debug("--- MEM stage display ---");
		Global::Debug(" - Entering stage - ");
		Global::Debug("pc                  : " + to_string(4000 + ((snapshot_before.pc_value - 4000) * 4)));
		Global::Debug("raw instruction     : " + snapshot_before.untouched_instruction);
		Global::Debug("op code             : " + Global::toString(snapshot_before.instruction.op_code));
		Global::Debug("destination reg tag : " + Global::toString(snapshot_before.instruction.dest.tag));
		Global::Debug("destination value   : N/A");
		Global::Debug("source 1 reg tag    : " + Global::toString(snapshot_before.instruction.src1.tag));
		Global::Debug("source 1 reg valid  : " + Global::toString(snapshot_before.instruction.src1.status));
		Global::Debug("source 1 reg value  : " + to_string(snapshot_before.instruction.src1.value));
		Global::Debug("source 2 reg tag    : " + Global::toString(snapshot_before.instruction.src2.tag));
		Global::Debug("source 2 reg valid  : " + Global::toString(snapshot_before.instruction.src2.status));
		Global::Debug("source 2 reg value  : " + to_string(snapshot_before.instruction.src2.value));
		Global::Debug("literal             : " + to_string(snapshot_before.instruction.literal_value));
		Global::Debug("....................");
		Global::Debug(" - Exiting stage - ");
		Global::Debug("pc                  : " + to_string(4000 + ((snapshot_after.pc_value - 4000) * 4)));
		Global::Debug("op code             : " + Global::toString(snapshot_after.instruction.op_code));
		Global::Debug("destination reg tag : " + Global::toString(snapshot_after.instruction.dest.tag));
		Global::Debug("destination value   : " + to_string(snapshot_after.instruction.dest.value));
		Global::Debug("source 1 reg tag    : " + Global::toString(snapshot_after.instruction.src1.tag));
		Global::Debug("source 1 reg valid  : " + Global::toString(snapshot_after.instruction.src1.status));
		Global::Debug("source 1 reg value  : " + to_string(snapshot_after.instruction.src1.value));
		Global::Debug("source 2 reg tag    : " + Global::toString(snapshot_after.instruction.src2.tag));
		Global::Debug("source 2 reg valid  : " + Global::toString(snapshot_after.instruction.src2.status));
		Global::Debug("source 2 reg value  : " + to_string(snapshot_after.instruction.src2.value));
		Global::Debug("literal             : " + to_string(snapshot_after.instruction.literal_value));
		Global::Debug("memory location     : " + to_string(snapshot_after.instruction.memory_location));
		Global::Debug("--- END MEM stage display ---");
	}
	else
	{
		Global::Debug("MEM STAGE --> No Instruction in Stage");
	}
}