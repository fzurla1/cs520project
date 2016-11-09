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
Global::apexStruct Memory::run(Global::apexStruct input_struct, Global::Register_Info * forward_bus, int * memory_array)
{
	Global::apexStruct output_struct = input_struct;
	snapshot_before = input_struct;

	//get forwarded info, if needed
	if (input_struct.instruction.destination_staus == 0)
	{
		if (forward_bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_tag == input_struct.instruction.destination_tag)
		{
			output_struct.instruction.destination_value = forward_bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_value;
			output_struct.instruction.destination_staus = Global::STATUS::VALID;
		}
	}

	if (output_struct.instruction.destination_staus == Global::STATUS::INVALID)
	{
		stalled = true;
	}
	else
	{
		if (output_struct.instruction.op_code == Global::OPCODE::LOAD)
		{
			output_struct.instruction.destination_value = memory_array[input_struct.instruction.memory_location];
			output_struct.instruction.destination_staus = 1;
			forward_bus[Global::FROM_MEMORY].reg_tag = output_struct.instruction.destination_tag;
			forward_bus[Global::FROM_MEMORY].reg_value = output_struct.instruction.destination_value;
		}
		else if (output_struct.instruction.op_code == Global::OPCODE::STORE)
		{
			memory_array[input_struct.instruction.memory_location] = output_struct.instruction.destination_value;

			//write bogus info into forward bus
			forward_bus[Global::FROM_MEMORY].reg_tag = Global::ARCH_REGISTERS::NA;
			forward_bus[Global::FROM_MEMORY].reg_value = -1;
		}
	}

	//save data for viewing
	snapshot_after = output_struct;

	return output_struct;
}

bool Memory::isStalled()
{
	return stalled;
}

void Memory::display()
{
	cout << endl
		<< "--- MEM stage display ---" << endl
		<< " - Entering stage - " << endl
		<< "pc                  : " << snapshot_before.pc_value << endl
		<< "op code             : " << snapshot_before.instruction.op_code << endl
		<< "destination reg tag : " << snapshot_before.instruction.destination_tag << endl
		<< "destination value   : N/A" << endl
		<< "source 1 reg tag    : " << snapshot_before.instruction.src1_tag << endl
		<< "source 1 reg valid  : " << snapshot_before.instruction.src1_valid << endl
		<< "source 1 reg value  : " << snapshot_before.instruction.src1_value << endl
		<< "source 2 reg tag    : " << snapshot_before.instruction.src2_tag << endl
		<< "source 2 reg valid  : " << snapshot_before.instruction.src2_valid << endl
		<< "source 2 reg value  : " << snapshot_before.instruction.src2_value << endl
		<< "literal             : " << snapshot_before.instruction.literal_value << endl
		<< "...................." << endl
		<< " - Exiting stage - " << endl
		<< "pc                  : " << snapshot_after.pc_value << endl
		<< "op code             : " << snapshot_after.instruction.op_code << endl
		<< "destination reg tag : " << snapshot_after.instruction.destination_tag << endl
		<< "destination value   : " << snapshot_after.instruction.destination_value << endl
		<< "source 1 reg tag    : " << snapshot_after.instruction.src1_tag << endl
		<< "source 1 reg valid  : " << snapshot_after.instruction.src1_valid << endl
		<< "source 1 reg value  : " << snapshot_after.instruction.src1_value << endl
		<< "source 2 reg tag    : " << snapshot_after.instruction.src2_tag << endl
		<< "source 2 reg valid  : " << snapshot_after.instruction.src2_valid << endl
		<< "source 2 reg value  : " << snapshot_after.instruction.src2_value << endl
		<< "literal             : " << snapshot_after.instruction.literal_value << endl
		<< "memory location     : " << snapshot_after.instruction.memory_location << endl
		<< "--- END MEM stage display ---" << endl;
}