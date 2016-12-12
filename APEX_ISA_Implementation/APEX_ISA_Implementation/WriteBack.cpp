#include "stdafx.h"
#include "WriteBack.h"

WriteBack::WriteBack()
{
//	myStructVector.resize(MAX_PORTS);
}


WriteBack::~WriteBack()
{
}

bool WriteBack::run(
	Global::Forwarding_Info(&Forward_Bus)[Global::FINAL_FORWARD_TYPE_TOTAL],
	Global::Reorder_Buffer(&ROB),
	Global::Register_Info * Register_File)
{
	bool HALT = false;
	Global::apexStruct garbage_struct;
	int size = myStructVector.size();
	snapshot_before = myStructVector;

	//if a branch has occured
	if (Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].updatePC == true)
	{
		for (int x = (size - 1); x >= 0; x--)
		{
			//if my PC is > than the branch, meaning that this instruction is later in the instruction
			//set, clear out my instruction
			if (myStructVector[x].pc_value > Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].pc_value)
			{
				myStructVector[x] = garbage_struct;
			}
		}
	}

	for (int x = (size -1); x >= 0; x--)
	{
		//make sure we have valid data
		if (myStructVector[x].pc_value != INT_MAX)
		{
			Forward_Bus[Global::FORWARD_TYPE(x + 4)].pc_value = myStructVector[x].pc_value;
			Forward_Bus[Global::FORWARD_TYPE(x + 4)].reg_info.tag = myStructVector[x].instruction.dest.tag;
			Forward_Bus[Global::FORWARD_TYPE(x + 4)].reg_info.value = myStructVector[x].instruction.dest.value;

			Register_File[myStructVector[x].instruction.dest.tag].status = Global::REGISTER_ALLOCATION::ALLOC_COMMIT;
			Register_File[myStructVector[x].instruction.dest.tag].value = myStructVector[x].instruction.dest.value;

			if (myStructVector[x].instruction.op_code == Global::OPCODE::HALT)
			{
				HALT = true;
			}

			myStructVector.pop_back();
		}
	}
	return HALT;
}

void WriteBack::setPipelineStruct(Global::apexStruct input_struct)
{
	if ((myStructVector.size() < Global::MAX_WRITEBACK_SIZE)
		&& ( input_struct.pc_value != INT_MAX))
	{
		myStructVector.push_back(input_struct);
	}
}

bool WriteBack::hasValidData()
{
	bool out = false;
	for (int x = (myStructVector.size() - 1); x >= 0; x--)
	{
		if (myStructVector[x].pc_value != INT_MAX)
		{
			out = true;
		}
	}

	return out;
}

string* WriteBack::getInstruction()
{
	int size = myStructVector.size();
	string * out = new string[myStructVector.size()];
	for (int x = (myStructVector.size() - 1); x >= 0; x--)
	{
		out[x] = myStructVector[x].untouched_instruction;
	}
	return out;
}

void WriteBack::display()
{
	if (snapshot_before.size() > 0)
	{
		Global::Output("WRITEBACK  - ");

		for (int x = (snapshot_before.size() - 1); x >= 0; x--)
		{
			//make sure we have valid data
			if (snapshot_before[x].pc_value != INT_MAX)
			{
				Global::Output("   INSTRUCTION " + to_string(x) + " - " + snapshot_before[x].untouched_instruction);
			}
		}
	}
	else
	{
		Global::Output("Writeback STAGE --> No Instruction in Stage");
	}
}
