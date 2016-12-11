#include "stdafx.h"
#include "Delay.h"

Delay::Delay()
{
}


Delay::~Delay()
{
}

Global::apexStruct Delay::run(bool (&Stalled_Stages)[Global::FINAL_STALLED_STAGE_TOTAL])
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

string Delay::getInstruction()
{
	return myStruct.untouched_instruction;
}

void Delay::display()
{
	//make sure we have valid data
	if (myStruct.pc_value != INT_MAX)
	{
		Global::Output("DELAY      - " + myStruct.untouched_instruction);
		/*
		Global::Output("\n--- Delay stage display ---\n - ENTERING STAGE -");
		Global::Output("pc                  : " + to_string(4000 + ((myStruct.pc_value -4000)* 4)));
		Global::Output("raw instruction     : " + myStruct.untouched_instruction);
		Global::Output("op code             : " + Global::toString(myStruct.instruction.op_code));
		Global::Output("destination reg tag : " + Global::toString(myStruct.instruction.dest.tag));
		Global::Output("destination value   : not ready");
		Global::Output("source 1 reg tag    : " + Global::toString(myStruct.instruction.src1.tag));
		Global::Output("source 1 reg valid  : " + Global::toString(myStruct.instruction.src1.status));

		if (myStruct.instruction.src1.status == Global::STATUS::INVALID)
			Global::Output("source 1 reg value  : invalid!");
		else
			Global::Output("source 1 reg value  : " + to_string(myStruct.instruction.src1.value));

		Global::Output("source 2 reg tag    : " + Global::toString(myStruct.instruction.src2.tag));
		Global::Output("source 2 reg valid  : " + Global::toString(myStruct.instruction.src2.status));

		if (!myStruct.instruction.src2.status)
			Global::Output("source 2 reg value  : invalid!");
		else
			Global::Output("source 2 reg value  : " + to_string(myStruct.instruction.src2.value));

		Global::Output("literal             : " + to_string(myStruct.instruction.literal_value));
		Global::Output("--- END Delay stage display ---");
		*/
	}
	else
	{
		Global::Output("Delay STAGE --> No Instruction in Stage");
	}
}
