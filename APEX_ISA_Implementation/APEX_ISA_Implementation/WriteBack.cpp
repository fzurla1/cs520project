#include "stdafx.h"
#include "WriteBack.h"

WriteBack::WriteBack()
{
}


WriteBack::~WriteBack()
{
}

bool WriteBack::run(
	Global::Register_Info(&Register_File)[Global::ARCH_REGISTER_COUNT],
	Global::Forwarding_Info(&Forward_Bus)[Global::FORWARDING_BUSES],
	int(&Most_Recent_Reg)[Global::ARCH_REGISTER_COUNT])
{
	bool HALT = false;
	snapshot_before = myStruct;

	//make sure we have valid data
	if (myStruct.pc_value != INT_MAX)
	{
		Register_File[myStruct.instruction.dest.tag].status = Global::STATUS::VALID;
		Register_File[myStruct.instruction.dest.tag].value = myStruct.instruction.dest.value;

		Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].pc_value = myStruct.pc_value;
		Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_info.tag = myStruct.instruction.dest.tag;
		Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_info.value = myStruct.instruction.dest.value;

		if (Most_Recent_Reg[myStruct.instruction.dest.tag] == myStruct.pc_value)
		{
			Most_Recent_Reg[myStruct.instruction.dest.tag] = 0;
		}

		if (myStruct.instruction.op_code == Global::OPCODE::HALT)
		{
			HALT = true;
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
