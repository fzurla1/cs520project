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
Global::apexStruct ALU1::run(Global::apexStruct input_struct, Global::Register_Info * forward_bus)
{
	Global::apexStruct output_struct = input_struct;
	snapshot_before = input_struct;

	//check source registers
	if (output_struct.instruction.src1_valid == Global::STATUS::INVALID)
	{
		if (forward_bus[Global::FROM_ALU2].reg_tag = output_struct.instruction.src1_tag)
		{
			forward_bus[Global::FROM_ALU2].reg_value = output_struct.instruction.src1_value;
			output_struct.instruction.src1_valid = Global::STATUS::VALID;
		}

		else if (forward_bus[Global::FROM_MEMORY].reg_tag = output_struct.instruction.src1_tag)
		{
			forward_bus[Global::FROM_MEMORY].reg_value = output_struct.instruction.src1_value;
			output_struct.instruction.src1_valid = Global::STATUS::VALID;
		}

		else if (forward_bus[Global::FROM_WRITEBACK].reg_tag = output_struct.instruction.src1_tag)
		{
			forward_bus[Global::FROM_WRITEBACK].reg_value = output_struct.instruction.src1_value;
			output_struct.instruction.src1_valid = Global::STATUS::VALID;
		}

		else
		{
			output_struct.instruction.src1_valid = Global::STATUS::INVALID;
		}
	}

	if (output_struct.instruction.src2_valid == Global::STATUS::INVALID)
	{
		if (forward_bus[Global::FROM_ALU2].reg_tag = output_struct.instruction.src2_tag)
		{
			forward_bus[Global::FROM_ALU2].reg_value = output_struct.instruction.src2_value;
			output_struct.instruction.src2_valid = Global::STATUS::VALID;
		}

		else if (forward_bus[Global::FROM_MEMORY].reg_tag = output_struct.instruction.src2_tag)
		{
			forward_bus[Global::FROM_MEMORY].reg_value = output_struct.instruction.src2_value;
			output_struct.instruction.src2_valid = Global::STATUS::VALID;
		}

		else if (forward_bus[Global::FROM_WRITEBACK].reg_tag = output_struct.instruction.src2_tag)
		{
			forward_bus[Global::FROM_WRITEBACK].reg_value = output_struct.instruction.src2_value;
			output_struct.instruction.src2_valid = Global::STATUS::VALID;
		}

		else
		{
			output_struct.instruction.src2_valid = Global::STATUS::INVALID;
		}
	}

	//verify source registers are valid to proceed
	if ((output_struct.instruction.src1_valid == Global::STATUS::VALID)
		&& (output_struct.instruction.src2_valid == Global::STATUS::VALID))
	{
		stalled = false;
	}

	//need to wait 
	else
	{
		stalled = true;
	}

	snapshot_after = output_struct;

	return output_struct;
}

bool ALU1::isStalled()
{
	return stalled;
}

void ALU1::display()
{
	Global::Debug("\n--- ALU1 stage display ---\n - ENTERING STAGE -");
	Global::Debug("pc                  : " + snapshot_before.pc_value);
	Global::Debug("op code             : " + Global::toString(snapshot_before.instruction.op_code));
	Global::Debug("destination reg tag : R" + Global::toString(snapshot_before.instruction.destination_tag));
	Global::Debug("destination value   : not ready");
	Global::Debug("source 1 reg tag    : " + Global::toString(snapshot_before.instruction.src1_tag));
	Global::Debug("source 1 reg valid  : " + Global::toString(snapshot_before.instruction.src1_valid));

	if (snapshot_before.instruction.src1_valid == Global::STATUS::INVALID)
		Global::Debug("source 1 reg value  : invalid!");
	else
		Global::Debug("source 1 reg value  : " + snapshot_before.instruction.src1_value);

	Global::Debug("source 2 reg tag    : " + Global::toString(snapshot_before.instruction.src2_tag));
	Global::Debug("source 2 reg valid  : " + Global::toString(snapshot_before.instruction.src2_valid));

	if (!snapshot_before.instruction.src2_valid)
		Global::Debug("source 2 reg value  : invalid!");
	else
		Global::Debug("source 2 reg value  : " + snapshot_before.instruction.src2_value);

	Global::Debug("literal             : " + snapshot_before.instruction.literal_value);
	Global::Debug(".....................");

	if (!stalled)
	{
		Global::Debug(" - EXITING STAGE -");
		Global::Debug("pc                  : " + snapshot_after.pc_value);
		Global::Debug("op code             : " + Global::toString(snapshot_after.instruction.op_code));
		Global::Debug("destination reg tag : " + Global::toString(snapshot_after.instruction.destination_tag));
		Global::Debug("destination value   : not ready");
		Global::Debug("source 1 reg tag    : " + Global::toString(snapshot_after.instruction.src1_tag));
		Global::Debug("source 1 reg valid  : " + Global::toString(snapshot_after.instruction.src1_valid));

		if (!snapshot_after.instruction.src1_valid)
			Global::Debug("source 1 reg value  : invalid!");
		else
			Global::Debug("source 1 reg value  : " + snapshot_after.instruction.src1_value);

		Global::Debug("source 2 reg tag    : " + Global::toString(snapshot_after.instruction.src2_tag));
		Global::Debug("source 2 reg valid  : " + Global::toString(snapshot_after.instruction.src2_valid));

		if (!snapshot_after.instruction.src2_valid)
			Global::Debug("source 2 reg value  : invalid!");
		else
			Global::Debug("source 2 reg value  : " + snapshot_after.instruction.src2_value);

		Global::Debug("literal             : " + snapshot_after.instruction.literal_value);
	}
	else
	{
		Global::Debug("**********************");
		Global::Debug("**  STAGE STALLED!  **");
		Global::Debug("**********************");
	}
	Global::Debug("--- END ALU1 stage display ---");
}