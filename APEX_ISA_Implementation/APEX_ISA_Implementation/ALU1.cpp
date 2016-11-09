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
			output_struct.instruction.src1_valid = true;
		}

		else if (forward_bus[Global::FROM_WRITEBACK].reg_tag = output_struct.instruction.src1_tag)
		{
			forward_bus[Global::FROM_WRITEBACK].reg_value = output_struct.instruction.src1_value;
			output_struct.instruction.src1_valid = true;
		}

		else
		{
			output_struct.instruction.src1_valid = false;
		}
	}

	if (output_struct.instruction.src2_valid == false)
	{
		if (forward_bus[Global::FROM_ALU2].reg_tag = output_struct.instruction.src2_tag)
		{
			forward_bus[Global::FROM_ALU2].reg_value = output_struct.instruction.src2_value;
			output_struct.instruction.src2_valid = true;
		}

		else if (forward_bus[Global::FROM_MEMORY].reg_tag = output_struct.instruction.src2_tag)
		{
			forward_bus[Global::FROM_MEMORY].reg_value = output_struct.instruction.src2_value;
			output_struct.instruction.src2_valid = true;
		}

		else if (forward_bus[Global::FROM_WRITEBACK].reg_tag = output_struct.instruction.src2_tag)
		{
			forward_bus[Global::FROM_WRITEBACK].reg_value = output_struct.instruction.src2_value;
			output_struct.instruction.src2_valid = true;
		}

		else
		{
			output_struct.instruction.src2_valid = false;
		}
	}

	//verify source registers are valid to proceed
	if ((output_struct.instruction.src1_valid == true)
		&& (output_struct.instruction.src2_valid == true))
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
	cout << endl
		<< "--- ALU1 stage display ---" << endl
		<< " - ENTERING STAGE -" << endl
		<< "pc                  : " << snapshot_before.pc_value << endl
		<< "op code             : " << snapshot_before.instruction.op_code << endl
		<< "destination reg tag : " << snapshot_before.instruction.destination_tag << endl
		<< "destination value   : not ready" << endl
		<< "source 1 reg tag    : " << snapshot_before.instruction.src1_tag << endl
		<< "source 1 reg valid  : " << snapshot_before.instruction.src1_valid << endl
		<< "source 1 reg value  : ";

	if (!snapshot_before.instruction.src1_valid)
		cout << "invalid!" << endl;
	else
		cout << snapshot_before.instruction.src1_value << endl;

	cout << "source 2 reg tag    : " << snapshot_before.instruction.src2_tag << endl
		<< "source 2 reg valid  : " << snapshot_before.instruction.src2_valid << endl
		<< "source 2 reg value  : ";

	if (!snapshot_before.instruction.src2_valid)
		cout << "invalid!" << endl;
	else
		cout << snapshot_before.instruction.src2_value << endl;

	cout << "literal             : " << snapshot_before.instruction.literal_value << endl
		<< "....................." << endl
		<< " - EXITING STAGE -" << endl
		<< "pc                  : " << snapshot_before.pc_value << endl
		<< "op code             : " << snapshot_before.instruction.op_code << endl
		<< "destination reg tag : " << snapshot_before.instruction.destination_tag << endl
		<< "destination value   : not ready" << endl
		<< "source 1 reg tag    : " << snapshot_before.instruction.src1_tag << endl
		<< "source 1 reg valid  : " << snapshot_before.instruction.src1_valid << endl
		<< "source 1 reg value  : ";

	if (!snapshot_before.instruction.src1_valid)
		cout << "invalid!" << endl;
	else
		cout << snapshot_before.instruction.src1_value << endl;

	cout << "source 2 reg tag    : " << snapshot_before.instruction.src2_tag << endl
		<< "source 2 reg valid  : " << snapshot_before.instruction.src2_valid << endl
		<< "source 2 reg value  : ";

	if (!snapshot_before.instruction.src2_valid)
		cout << "invalid!" << endl;
	else
		cout << snapshot_before.instruction.src2_value << endl;

	cout << "literal             : " << snapshot_before.instruction.literal_value << endl
		<< "--- END ALU1 stage display ---" << endl;
}