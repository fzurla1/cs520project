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
	//assue no stall, and no branch
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
				Global::Debug("-- Branch Taken! BAL-- ");
				break;
			case Global::OPCODE::BNZ:
				if ((!Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].flag == Global::FLAGS::ZERO)
					&& (Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].pc_value == myStruct.branch_waiting_to_complete))
				{
					Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].opcode = Global::OPCODE::BNZ;
					Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].updatePC = true;
					Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].target = myStruct.instruction.literal_value;
					Global::Debug("-- Branch Taken! BNZ-- ");
				}
				else
				{
					Stalled_Stages[Global::STALLED_STAGE::BRANCH] = true;
				}
				break;
			case Global::OPCODE::BZ:
				if ((Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].flag == Global::FLAGS::ZERO)
					&& (Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].pc_value == myStruct.branch_waiting_to_complete))
				{
					Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].opcode = Global::OPCODE::BZ;
					Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].updatePC = true;
					Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].target = myStruct.instruction.literal_value;
					Global::Debug("-- Branch Taken! BZ-- ");
				}
				else
				{
					Stalled_Stages[Global::STALLED_STAGE::BRANCH] = true;
				}
				break;
			case Global::OPCODE::JUMP:
				Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].opcode = Global::OPCODE::JUMP;
				Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].updatePC = true;
				Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].target = X.value + myStruct.instruction.literal_value/4;
				Global::Debug("-- Branch Taken! JUMP-- ");
				break;
		}
		x_value = X.value;
		updatePC = Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].updatePC;
		target = Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].target;
	}

	//if updating PC from branch, store off current PC value too
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

bool Branch::hasValidData()
{
	return (myStruct.pc_value != INT_MAX);
}

string Branch::getInstruction()
{
	return myStruct.untouched_instruction;
}

void Branch::display()
{
	//make sure we have valid data
	if (myStruct.pc_value != INT_MAX)
	{
		Global::Debug("BRANCH     - " + myStruct.untouched_instruction);
		/*
		Global::Debug("\n--- Branch stage display ---\n - ENTERING STAGE -");
		Global::Debug("pc                  : " + to_string(4000 + ((myStruct.pc_value - 4000) * 4)));
		Global::Debug("raw instruction     : " + myStruct.untouched_instruction);
		Global::Debug("op code             : " + Global::toString(myStruct.instruction.op_code));
		if (updatePC)
		{
			Global::Debug("Branch Taken!");
			Global::Debug("X                   : " + to_string(4000 + ((x_value - 4000) * 4)));
			Global::Debug("target              : " + to_string(target));
		}
		Global::Debug("--- END Branch stage display ---");
		*/
	}
	else
	{
		Global::Debug("Branch STAGE --> No Instruction in Stage");
	}
}
