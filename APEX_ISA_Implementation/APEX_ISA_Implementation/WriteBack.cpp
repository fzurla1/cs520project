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

	for (int x = (myStructVector.size()-1); x >= 0; x--)
	{
		//make sure we have valid data
		if (myStructVector[x].pc_value != INT_MAX)
		{
			Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].pc_value = myStructVector[x].pc_value;
			Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_info.tag = myStructVector[x].instruction.dest.tag;
			Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_info.value = myStructVector[x].instruction.dest.value;

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
	if (myStructVector.size() < Global::MAX_WRITEBACK_SIZE)
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
	bool has_instruction = false;

	Global::Output("WRITEBACK  - ");

	for (int x = (myStructVector.size() - 1); x >= 0; x--)
	{
		//make sure we have valid data
		if (myStructVector[x].pc_value != INT_MAX)
		{
			Global::Output("   INSTRUCTION " + to_string(x) + " - " + myStructVector[x].untouched_instruction);
		}
	}
	if ( !has_instruction)
	{
		Global::Output("Writeback STAGE --> No Instruction in Stage");
	}
}
