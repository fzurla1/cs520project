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
	Global::apexStruct output_struct = myStruct;
	snapshot_before = myStruct;

	//make sure we have valid data
	if (myStruct.pc_value != INT_MAX)
	{

		if (!Stalled_Stages[Global::STALLED_STAGE::ALU2])
		{
			//assume not stalled
			Stalled_Stages[Global::STALLED_STAGE::ALU1] = false;

			//verify source registers are valid to proceed
			if (((output_struct.instruction.src1.status == Global::STATUS::VALID) && (output_struct.instruction.src2.status == Global::STATUS::VALID))
				|| ((output_struct.instruction.op_code == Global::OPCODE::STORE) && (output_struct.instruction.src2.status == Global::STATUS::VALID)))
			{
				Stalled_Stages[Global::STALLED_STAGE::ALU1] = false;
			}

			//need to wait 
			else
			{
				Stalled_Stages[Global::STALLED_STAGE::ALU1] = true;
			}
		}
		else
		{
			Stalled_Stages[Global::STALLED_STAGE::ALU1] = true;
		}
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
	if (myStruct.pc_value != INT_MAX)
	{
		Global::Debug("ALU1       - " + snapshot_before.untouched_instruction);
	}
	else
	{
		Global::Debug("ALU 1 STAGE --> No Instruction in Stage");
	}
}