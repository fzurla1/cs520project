#include "stdafx.h"
#include "Delay.h"

Delay::Delay()
{
}


Delay::~Delay()
{
}

Global::apexStruct Delay::run(bool (&Stalled_Stages)[Global::TOTAL_STAGES])
{
	Global::apexStruct output_struct;

	//make sure we have valid data
	if (myStruct.pc_value != INT_MAX)
	{
		if (!Stalled_Stages[Global::STALLED_STAGE::MEMORY])
		{
			Stalled_Stages[Global::STALLED_STAGE::DELAY] = false;
			output_struct = myStruct;
		}
		else
		{
			Stalled_Stages[Global::STALLED_STAGE::DELAY] = true;
		}
	}
	return output_struct;
}

void Delay::setPipelineStruct(Global::apexStruct input_struct)
{
	myStruct = input_struct;
}

bool Delay::hasValidData()
{
	return (myStruct.pc_value != INT_MAX);
}

void Delay::display()
{
	//make sure we have valid data
	if (myStruct.pc_value != INT_MAX)
	{
		Global::Debug("\n--- Delay stage display ---\n - ENTERING STAGE -");
		Global::Debug("pc                  : " + to_string(4000 + ((myStruct.pc_value -4000)* 4)));
		Global::Debug("raw instruction     : " + myStruct.untouched_instruction);
		Global::Debug("op code             : " + Global::toString(myStruct.instruction.op_code));
		Global::Debug("destination reg tag : " + Global::toString(myStruct.instruction.dest.tag));
		Global::Debug("destination value   : not ready");
		Global::Debug("source 1 reg tag    : " + Global::toString(myStruct.instruction.src1.tag));
		Global::Debug("source 1 reg valid  : " + Global::toString(myStruct.instruction.src1.status));

		if (myStruct.instruction.src1.status == Global::STATUS::INVALID)
			Global::Debug("source 1 reg value  : invalid!");
		else
			Global::Debug("source 1 reg value  : " + to_string(myStruct.instruction.src1.value));

		Global::Debug("source 2 reg tag    : " + Global::toString(myStruct.instruction.src2.tag));
		Global::Debug("source 2 reg valid  : " + Global::toString(myStruct.instruction.src2.status));

		if (!myStruct.instruction.src2.status)
			Global::Debug("source 2 reg value  : invalid!");
		else
			Global::Debug("source 2 reg value  : " + to_string(myStruct.instruction.src2.value));

		Global::Debug("literal             : " + to_string(myStruct.instruction.literal_value));
		Global::Debug("--- END Delay stage display ---");
	}
	else
	{
		Global::Debug("Delay STAGE --> No Instruction in Stage");
	}
}
