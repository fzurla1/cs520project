#include "stdafx.h"
#include "Dispatch.h"


Dispatch::Dispatch()
{
}


Dispatch::~Dispatch()
{
}

Global::apexStruct Dispatch::run(
	Global::Register_Info *Register_File,
	int URF_SIZE,
	Global::Forwarding_Info(&Forward_Bus)[Global::FORWARD_TYPE::FINAL_FORWARD_TYPE_TOTAL],
	bool(&Stalled_Stages)[Global::STALLED_STAGE::FINAL_STALLED_STAGE_TOTAL],
	Global::Reorder_Buffer(&ROB),
	Global::Front_End_Rename_Entry(&Front_End_RAT)[Global::ARCH_REGISTERS::FINAL_ARCH_REGISTERS_ITEM])
{
	Global::apexStruct output_struct;

	int arch_src1;
	int arch_src2;

	//if a branch has occured
	//if my PC is > than the branch, meaning that this instruction is later in the instruction
	//set, clear out my instruction
	if ((Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].updatePC)
		&& (Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].pc_value < myStruct.pc_value))
	{
		myStruct.clear();
		return output_struct;
	}
	output_struct = myStruct;
	snapshot_before = myStruct;

	arch_src1 = output_struct.instruction.src1.archreg;
	arch_src2 = output_struct.instruction.src2.archreg;

	//make sure we have valid data
	if (myStruct.pc_value != INT_MAX)
	{
		//For sources not valid, check again in the forward bus, ROB or register file
		if (output_struct.instruction.src1.status == Global::STATUS::INVALID)
		{
			output_struct.instruction.src1.status = Global::STATUS::VALID;

			//FOR SRC1
			//if the entry is not complete, but it is executing or waiting, check forwarding bus
			//check from ALU2
			if (Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].reg_info.tag == output_struct.instruction.src1.tag)
			{
				output_struct.instruction.src1.value = Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].reg_info.value;
			}
			//check from memory
			else if (Forward_Bus[Global::FORWARD_TYPE::FROM_MULTIPLY].reg_info.tag == output_struct.instruction.src1.tag)
			{
				output_struct.instruction.src1.value = Forward_Bus[Global::FORWARD_TYPE::FROM_MULTIPLY].reg_info.value;
			}
			//check from writeback
			else if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK1].reg_info.tag == output_struct.instruction.src1.tag)
			{
				output_struct.instruction.src1.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK1].reg_info.value;
			}
			//check from writeback
			else if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK2].reg_info.tag == output_struct.instruction.src1.tag)
			{
				output_struct.instruction.src1.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK2].reg_info.value;
			}
			//check from writeback
			else if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK3].reg_info.tag == output_struct.instruction.src1.tag)
			{
				output_struct.instruction.src1.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK3].reg_info.value;
			}
			//check from writeback
			else if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK4].reg_info.tag == output_struct.instruction.src1.tag)
			{
				output_struct.instruction.src1.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK4].reg_info.value;
			}
			//check ROB and forwarding buses
			//if tag matches what we are looking for, and that instruction is the most recent to update 
			//the register, then grab that value, otherwise, stall.
			else if (ROB.entries[Front_End_RAT[output_struct.instruction.src1.archreg].rob_loc].alloc == Global::ROB_ALLOCATION::COMPLETE)
			{
				//get ROB entry location
				output_struct.instruction.src1.rob_loc = Front_End_RAT[arch_src1].reg;
				output_struct.instruction.src1.value = ROB.entries[Front_End_RAT[arch_src1].rob_loc].result;
			}
			//look to FE RAT for where data is stored
			else if ((Front_End_RAT[arch_src1].src_bit == Global::SOURCES::REGISTER_FILE)
				&& (Register_File[Front_End_RAT[arch_src1].reg].status == Global::REGISTER_ALLOCATION::ALLOC_COMMIT))
			{
				output_struct.instruction.src1.value = Register_File[output_struct.instruction.src1.tag].value;
			}
			else
			{
				output_struct.instruction.src1.status = Global::STATUS::INVALID;
			}
		}

		if (output_struct.instruction.src2.status == Global::STATUS::INVALID)
		{
			output_struct.instruction.src2.status = Global::STATUS::VALID;

			//FOR SRC2
			//if the entry is not complete, but it is executing or waiting, check forwarding bus
			//check from ALU2
			if (Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].reg_info.tag == output_struct.instruction.src2.tag)
			{
				output_struct.instruction.src2.value = Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].reg_info.value;
			}
			//check from memory
			else if (Forward_Bus[Global::FORWARD_TYPE::FROM_MULTIPLY].reg_info.tag == output_struct.instruction.src2.tag)
			{
				output_struct.instruction.src2.value = Forward_Bus[Global::FORWARD_TYPE::FROM_MULTIPLY].reg_info.value;
			}
			//check from writeback
			else if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK1].reg_info.tag == output_struct.instruction.src2.tag)
			{
				output_struct.instruction.src2.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK1].reg_info.value;
			}
			//check from writeback
			else if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK2].reg_info.tag == output_struct.instruction.src2.tag)
			{
				output_struct.instruction.src2.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK2].reg_info.value;
			}
			//check from writeback
			else if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK3].reg_info.tag == output_struct.instruction.src2.tag)
			{
				output_struct.instruction.src2.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK3].reg_info.value;
			}
			//check from writeback
			else if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK4].reg_info.tag == output_struct.instruction.src2.tag)
			{
				output_struct.instruction.src2.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK4].reg_info.value;
			}
			//check ROB and forwarding buses
			//if tag matches what we are looking for, and that instruction is the most recent to update 
			//the register, then grab that value, otherwise, stall.
			else if (ROB.entries[Front_End_RAT[arch_src2].rob_loc].alloc == Global::ROB_ALLOCATION::COMPLETE)
			{
				//get ROB entry location
				output_struct.instruction.src2.rob_loc = Front_End_RAT[arch_src1].reg;
				output_struct.instruction.src2.value = ROB.entries[Front_End_RAT[arch_src1].rob_loc].result;
			}
			//look to FE RAT for where data is stored
			else if ((Front_End_RAT[arch_src2].src_bit == Global::SOURCES::REGISTER_FILE)
				&& (Register_File[Front_End_RAT[arch_src2].reg].status == Global::REGISTER_ALLOCATION::ALLOC_COMMIT))
			{
				output_struct.instruction.src2.value = Register_File[output_struct.instruction.src2.tag].value;
			}
			else
			{
				output_struct.instruction.src2.status = Global::STATUS::INVALID;
			}
		}
	}
	
	snapshot_after = output_struct;
	return output_struct;
}


void Dispatch::setPipelineStruct(Global::apexStruct input_struct)
{
	myStruct = input_struct;
}


bool Dispatch::hasValidData()
{
	return (myStruct.pc_value != INT_MAX);
}

string Dispatch::getInstruction()
{
	return myStruct.untouched_instruction;
}

void Dispatch::display()
{
	//make sure we have valid data
	if (snapshot_before.pc_value != INT_MAX)
	{
		Global::Output("Rename2/Dispatch  - " + snapshot_before.untouched_instruction);
	}
	else
	{
		Global::Output("Rename2/Dispatch STAGE --> No Instruction in Stage");
	}
}