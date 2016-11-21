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
Global::apexStruct ALU1::run( Global::Forwarding_Info(&Forward_Bus)[Global::FORWARDING_BUSES],
	bool(&Stalled_Stages)[Global::TOTAL_STAGES])
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
		/*
		Global::Debug("\n--- ALU1 stage display ---\n - ENTERING STAGE -");
		Global::Debug("pc                  : " + to_string(4000 + ((snapshot_before.pc_value -4000)* 4)));
		Global::Debug("raw instruction     : " + snapshot_before.untouched_instruction);
		
		Global::Debug("op code             : " + Global::toString(snapshot_before.instruction.op_code));
		Global::Debug("destination reg tag : " + Global::toString(snapshot_before.instruction.dest.tag));
		Global::Debug("destination value   : not ready");
		Global::Debug("source 1 reg tag    : " + Global::toString(snapshot_before.instruction.src1.tag));
		Global::Debug("source 1 reg valid  : " + Global::toString(snapshot_before.instruction.src1.status));

		if (snapshot_before.instruction.src1.status == Global::STATUS::INVALID)
			Global::Debug("source 1 reg value  : invalid!");
		else
			Global::Debug("source 1 reg value  : " + to_string(snapshot_before.instruction.src1.value));

		Global::Debug("source 2 reg tag    : " + Global::toString(snapshot_before.instruction.src2.tag));
		Global::Debug("source 2 reg valid  : " + Global::toString(snapshot_before.instruction.src2.status));

		if (!snapshot_before.instruction.src2.status)
			Global::Debug("source 2 reg value  : invalid!");
		else
			Global::Debug("source 2 reg value  : " + to_string(snapshot_before.instruction.src2.value));

		Global::Debug("literal             : " + to_string(snapshot_before.instruction.literal_value));
		Global::Debug(".....................");

		Global::Debug(" - EXITING STAGE -");
		Global::Debug("pc                  : " + to_string(4000 + ((snapshot_after.pc_value - 4000) * 4)));
		Global::Debug("op code             : " + Global::toString(snapshot_after.instruction.op_code));
		Global::Debug("destination reg tag : " + Global::toString(snapshot_after.instruction.dest.tag));
		Global::Debug("destination value   : not ready");
		Global::Debug("source 1 reg tag    : " + Global::toString(snapshot_after.instruction.src1.tag));
		Global::Debug("source 1 reg valid  : " + Global::toString(snapshot_after.instruction.src1.status));

		if (!snapshot_after.instruction.src1.status)
			Global::Debug("source 1 reg value  : invalid!");
		else
			Global::Debug("source 1 reg value  : " + to_string(snapshot_after.instruction.src1.value));

		Global::Debug("source 2 reg tag    : " + Global::toString(snapshot_after.instruction.src2.tag));
		Global::Debug("source 2 reg valid  : " + Global::toString(snapshot_after.instruction.src2.status));

		if (!snapshot_after.instruction.src2.status)
			Global::Debug("source 2 reg value  : invalid!");
		else
			Global::Debug("source 2 reg value  : " + to_string(snapshot_after.instruction.src2.value));

		Global::Debug("literal             : " + to_string(snapshot_after.instruction.literal_value));

		Global::Debug("--- END ALU1 stage display ---");
		*/
	}
	else
	{
		Global::Debug("ALU 1 STAGE --> No Instruction in Stage");
	}
}