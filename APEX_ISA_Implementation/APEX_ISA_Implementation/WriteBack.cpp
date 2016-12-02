#include "stdafx.h"
#include "WriteBack.h"

WriteBack::WriteBack()
{
}


WriteBack::~WriteBack()
{
}

bool WriteBack::run(
	Global::Forwarding_Info(&Forward_Bus)[Global::FINAL_FORWARD_TYPE_TOTAL],
	Global::Reorder_Buffer(&ROB),
	Global::Register_Info * Register_File,
	//int(&Most_Recent_Reg)[Global::FINAL_ARCH_REGISTERS_ITEM]);
	Global::Rename_Table(&Back_End_RAT))
{
	bool HALT = false;

	for (int x = (myStructVector.size()-1); x <= 0; x--)
	{
		//make sure we have valid data
		if (myStructVector[x].pc_value != INT_MAX)
		{
			Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].pc_value = myStructVector[x].pc_value;
			Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_info.tag = myStructVector[x].instruction.dest.tag;
			Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_info.value = myStructVector[x].instruction.dest.value;

			Register_File[myStructVector[x].instruction.dest.tag].status = Global::REGISTER_ALLOCATION::ALLOC_NO_COMMIT;
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
	for (int x = 0; x < (myStructVector.size() - 1); x++)
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
	for (int x = 0; x < size; x++)
	{
		out[x] = myStructVector[x].untouched_instruction;
	}
	return out;
}

void WriteBack::display()
{
	bool has_instruction = false;

	Global::Debug("WRITEBACK  - ");

	for (int x = 0; x < Global::MAX_WRITEBACK_SIZE; x++)
	{
		//make sure we have valid data
		if (myStructVector[x].pc_value != INT_MAX)
		{
			Global::Debug("   INSTRUCTION " + to_string(x) + " - " + myStructVector[x].untouched_instruction);
		}
	}
	if ( !has_instruction)
	{
		Global::Debug("Writeback STAGE --> No Instruction in Stage");
	}
}
