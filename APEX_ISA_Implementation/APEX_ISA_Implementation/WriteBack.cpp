#include "stdafx.h"
#include "WriteBack.h"

#define MAXSIZE 3

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
	snapshot_before = myStruct;

	//make sure we have valid data
	if (myStruct.pc_value != INT_MAX)
	{

		ROB.entries[myStruct.instruction.dest.rob_loc].alloc = Global::ROB_ALLOCATION::COMPLETE;
		ROB.entries[myStruct.instruction.dest.rob_loc].result = myStruct.instruction.dest.value;
		ROB.entries[myStruct.instruction.dest.rob_loc].flags = myStruct.instruction.flag;

		Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].pc_value = myStruct.pc_value;
		Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_info.tag = myStruct.instruction.dest.tag;
		Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_info.value = myStruct.instruction.dest.value;

		for (int x = 0; x < myStructVector.size(); x++)
		{
			Register_File[myStructVector[x].instruction.dest.tag].status = Global::REGISTER_ALLOCATION::ALLOC_NO_COMMIT;
			Register_File[myStructVector[x].instruction.dest.tag].value = myStructVector[x].instruction.dest.value;
		}
		/*
		//update register file
		Register_File[ROB.entries[ROB.head].destReg].status = Global::REGISTER_ALLOCATION::ALLOC_COMMIT;
		Register_File[ROB.entries[ROB.head].destReg].value = ROB.entries[ROB.head].result;
		*/

		if (myStruct.instruction.op_code == Global::OPCODE::HALT)
		{
			HALT = true;
		}

		//update ROB
		if (ROB.head != ROB.tail)
		{
			//update back end RAT
			Back_End_RAT.reg[ROB.entries[ROB.head].destArchReg] = ROB.entries[ROB.head].destReg;
			Back_End_RAT.rob_loc[ROB.entries[ROB.head].destArchReg] = -1;
			Back_End_RAT.src_bit[ROB.entries[ROB.head].destArchReg] = Global::SOURCES::REGISTER_FILE;

			

			//update ROB head pointer
			ROB.head = (ROB.head + 1) % Global::ROB_SIZE;
		}
	}
	return HALT;
}

void WriteBack::setPipelineStruct(Global::apexStruct input_struct)
{
	if (myStructVector.size() < MAXSIZE)
	{
		myStructVector.push_back(input_struct);
	}
}

bool WriteBack::hasValidData()
{
	return (myStruct.pc_value != INT_MAX);
}

string WriteBack::getInstruction()
{
	return myStruct.untouched_instruction;
}

void WriteBack::display()
{
	//make sure we have valid data
	if (myStruct.pc_value != INT_MAX)
	{
		Global::Debug("WRITEBACK  - " + snapshot_before.untouched_instruction);
	}
	else
	{
		Global::Debug("Writeback STAGE --> No Instruction in Stage");
	}
}
