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

			//update register file
			Register_File[ROB.entries[ROB.head].destReg].status = Global::REGISTER_ALLOCATION::ALLOC_COMMIT;
			Register_File[ROB.entries[ROB.head].destReg].value = ROB.entries[ROB.head].result;

			//update ROB head pointer
			ROB.head = (ROB.head + 1) % Global::ROB_SIZE;
		}
	}
	return HALT;
}

void WriteBack::setPipelineStruct(Global::apexStruct input_struct)
{
	myStruct = input_struct;
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
		/*
		Global::Debug("\n--- Writeback stage display ---\n - ENTERING STAGE -");
		Global::Debug("pc                  : " + to_string(4000 + ((snapshot_before.pc_value - 4000) * 4)));
		Global::Debug("raw instruction     : " + snapshot_before.untouched_instruction);
		Global::Debug("op code             : " + Global::toString(snapshot_before.instruction.op_code));
		Global::Debug("destination reg tag : " + Global::toString(snapshot_before.instruction.dest.tag));
		Global::Debug("destination value   : " + to_string(snapshot_before.instruction.dest.value));
		Global::Debug("source 1 reg tag    : " + Global::toString(snapshot_before.instruction.src1.tag));
		Global::Debug("source 1 reg valid  : " + Global::toString(snapshot_before.instruction.src1.status));

		if (snapshot_before.instruction.src1.status == Global::STATUS::INVALID)
			Global::Debug("source 1 reg value  : invalid!");
		else
			Global::Debug("source 1 reg value  : " + to_string(snapshot_before.instruction.src1.value));

		Global::Debug("source 2 reg tag    : " + Global::toString(snapshot_before.instruction.src2.tag));
		Global::Debug("source 2 reg valid  : " + Global::toString(snapshot_before.instruction.src2.status));

		if (!snapshot_before.instruction.src2.status)
			Global::Debug("source 2 reg value  : invalid!");
		else
			Global::Debug("source 2 reg value  : " + to_string(snapshot_before.instruction.src2.value));

		Global::Debug("literal             : " + to_string(snapshot_before.instruction.literal_value));
		Global::Debug("--- END Writeback stage display ---");
		*/
	}
	else
	{
		Global::Debug("Writeback STAGE --> No Instruction in Stage");
	}
}
