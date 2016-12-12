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
	int(&Memory_Array)[Global::MEMORY_SIZE], Global::ROB)
{
	Global::apexStruct output_struct = myStruct;
	snapshot_before = myStruct;
	Stalled_Stages[Global::STALLED_STAGE::LS2] = false;

	//make sure we have valid data
	if (myStruct.pc_value != INT_MAX)
	{
		//Stall if ROB entry is not head
		if ()
	}

}

void LS2::setPipelineStruct(Global::apexStruct input_struct)
{
	myStruct = input_struct;
}

bool LS2::hasValidData()
{
	return (myStruct.pc_value != INT_MAX);
}

