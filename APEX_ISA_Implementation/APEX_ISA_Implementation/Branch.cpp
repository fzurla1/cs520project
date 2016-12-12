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
	Global::Forwarding_Info(&Forward_Bus)[Global::FINAL_FORWARD_TYPE_TOTAL],
	bool(&Stalled_Stages)[Global::FINAL_STALLED_STAGE_TOTAL],
	Global::Register_Info(&X),
	Global::Reorder_Buffer(&ROB))
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
			//Update ROB
			ROB.entries[output_struct.instruction.dest.rob_loc].alloc = Global::ROB_ALLOCATION::COMPLETE;

			case Global::OPCODE::BAL:
				Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].opcode = Global::OPCODE::BAL;
				Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].updatePC = true;
				Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].target = myStruct.instruction.dest.value + myStruct.instruction.literal_value;
				X.value = PC + 1;
				ROB.entries[output_struct.instruction.dest.rob_loc].result = PC + 1;
				output_struct.instruction.dest.value = PC + 1;
				Global::Output("-- Branch Taken! BAL-- ");
				break;
			case Global::OPCODE::BNZ:
				if (myStruct.instruction.flag == Global::FLAGS::ZERO)
				{
					Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].opcode = Global::OPCODE::BNZ;
					Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].updatePC = true;
					Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].target = myStruct.instruction.literal_value;
					Global::Output("-- Branch Taken! BNZ-- ");
				}
				else
				{
					Global::Output("-- Branch NOT Taken.-- ");
				}
				break;
			case Global::OPCODE::BZ:
				if (myStruct.instruction.flag == Global::FLAGS::ZERO)
				{
					Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].opcode = Global::OPCODE::BZ;
					Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].updatePC = true;
					Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].target = myStruct.instruction.literal_value;
					Global::Output("-- Branch Taken! BZ-- ");
				}
				else
				{
					Global::Output("-- Branch NOT Taken.-- ");
				}
				break;
			case Global::OPCODE::JUMP:
				Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].opcode = Global::OPCODE::JUMP;
				Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].updatePC = true;
				Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].target = X.value + myStruct.instruction.literal_value/4;
				Global::Output("-- Branch Taken! JUMP-- ");
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
		Global::Output("BRANCH     - " + myStruct.untouched_instruction);
		/*
		Global::Output("\n--- Branch stage display ---\n - ENTERING STAGE -");
		Global::Output("pc                  : " + to_string(4000 + ((myStruct.pc_value - 4000) * 4)));
		Global::Output("raw instruction     : " + myStruct.untouched_instruction);
		Global::Output("op code             : " + Global::toString(myStruct.instruction.op_code));
		if (updatePC)
		{
			Global::Output("Branch Taken!");
			Global::Output("X                   : " + to_string(4000 + ((x_value - 4000) * 4)));
			Global::Output("target              : " + to_string(target));
		}
		Global::Output("--- END Branch stage display ---");
		*/
	}
	else
	{
		Global::Output("Branch STAGE --> No Instruction in Stage");
	}
}
