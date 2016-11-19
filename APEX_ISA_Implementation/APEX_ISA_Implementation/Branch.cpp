#include "stdafx.h"
#include "Branch.h"

Branch::Branch()
{
}


Branch::~Branch()
{
}


Global::apexStruct Branch::run(
	int PC,
	Global::Forwarding_Info(&Forward_Bus)[Global::FORWARDING_BUSES],
	bool(&Stalled_Stages)[Global::TOTAL_STAGES],
	Global::Register_Info(&X))
{
	Global::apexStruct output_struct = myStruct;
	Stalled_Stages[Global::STALLED_STAGE::BRANCH] = false;
	Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].updatePC = false;
	Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].opcode = Global::OPCODE::NONE;

	//make sure we have valid data
	if (myStruct.pc_value != INT_MAX)
	{
		switch (myStruct.instruction.op_code)
		{
			case Global::OPCODE::BAL:
				Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].opcode = Global::OPCODE::BAL;
				Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].updatePC = true;
				Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].target = myStruct.instruction.dest.value + myStruct.instruction.literal_value;
				X.value = PC + 1;
				break;
			case Global::OPCODE::BNZ:
				if (!Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].flag == Global::FLAGS::ZERO)
				{
					Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].opcode = Global::OPCODE::BNZ;
					Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].updatePC = true;
					Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].target = myStruct.instruction.literal_value;
				}
				break;
			case Global::OPCODE::BZ:
				if (Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].flag == Global::FLAGS::ZERO)
				{
					Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].opcode = Global::OPCODE::BZ;
					Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].updatePC = true;
					Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].target = myStruct.instruction.literal_value;
				}
				break;
			case Global::OPCODE::JUMP:
				Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].opcode = Global::OPCODE::JUMP;
				Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].updatePC = true;
				Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].target = X.value + myStruct.instruction.literal_value;
				break;
		}
	}

	if (Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].updatePC)
	{
		Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].pc_value = myStruct.pc_value;
	}

	return output_struct;
}

void Branch::setPipelineStruct(Global::apexStruct input_struct)
{
	myStruct = input_struct;
}

void Branch::display()
{
	//make sure we have valid data
	if (myStruct.pc_value != INT_MAX)
	{
		Global::Debug("\n--- Branch stage display ---\n - ENTERING STAGE -");
		Global::Debug("pc                  : " + to_string(4000 + ((myStruct.pc_value - 4000) * 4)));
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
		Global::Debug("--- END Branch stage display ---");
	}
	else
	{
		Global::Debug("Branch STAGE --> No Instruction in Stage");
	}
}
