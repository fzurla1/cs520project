#include "stdafx.h"
#include "ALU1.h"

using namespace std;

ALU1::ALU1()
{

}

ALU1::~ALU1()
{

}

//	This function will:
//	1) Read the forward bus for missing register values
//	2) update the apexStruct for the ALU2 phase
Global::apexStruct ALU1::run( Global::Forwarding_Info(&Forward_Bus)[Global::FINAL_FORWARD_TYPE_TOTAL],
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
		//assume not stalled
		Stalled_Stages[Global::STALLED_STAGE::ALU1] = false;
	}
	snapshot_after = output_struct;

	return output_struct;
}

void ALU1::setPipelineStruct(Global::apexStruct input_struct)
{
	myStruct = input_struct;
}

bool ALU1::hasValidData()
{
	return (myStruct.pc_value != INT_MAX);
}

string ALU1::getInstruction()
{
	return myStruct.untouched_instruction;
}

void ALU1::display()
{
	//make sure we have valid data
	if (snapshot_before.pc_value != INT_MAX)
	{
		Global::Output("ALU1       - " + snapshot_before.untouched_instruction);
	}
	else
	{
		Global::Output("ALU 1 STAGE --> No Instruction in Stage");
	}
}