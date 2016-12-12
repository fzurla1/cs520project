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
	Stalled_Stages[Global::STALLED_STAGE::LS1] = false;

	if (myStruct.pc_value != INT_MAX)
	{
		//Get memory location
		myStruct.instruction.memory_location = myStruct.instruction.memory_location / 4;
	}

	snapshot_after = output_struct;

	return output_struct;
}

void LS1::display()
{

}