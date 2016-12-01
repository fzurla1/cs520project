#include "stdafx.h"
#include "Multiply.h"


Multiply::Multiply()
{
}


Multiply::~Multiply()
{
}


Global::apexStruct Multiply::run(Global::Forwarding_Info(&Forward_Bus)[Global::FINAL_FORWARD_TYPE_TOTAL],
	bool(&Stalled_Stages)[Global::FINAL_STALLED_STAGE_TOTAL])
{
	Global::apexStruct output_struct = myStruct;
	snapshot_before = myStruct;

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
		Global::Debug("MULTIPLY  - " + snapshot_before.untouched_instruction);
	}
	else
	{
		Global::Debug("MULTIPLY 2 STAGE --> No Instruction in Stage");
	}
}