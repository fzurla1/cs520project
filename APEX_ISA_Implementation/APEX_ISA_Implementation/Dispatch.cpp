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
	Global::apexStruct output_struct = myStruct;
	snapshot_before = myStruct;

	//make sure we have valid data
	if (myStruct.pc_value != INT_MAX)
	{
		//For sources not valid, check again in the forward bus, ROB or register file
		if (output_struct.instruction.src1.status == Global::STATUS::INVALID)
		{
			output_struct.instruction.src1.status = Global::STATUS::VALID;

			//look to FE RAT for where data is stored
			if ((Front_End_RAT[output_struct.instruction.src1.archreg].src_bit == Global::SOURCES::REGISTER_FILE)
				&& (Register_File[Front_End_RAT[output_struct.instruction.src1.archreg].reg].status == Global::REGISTER_ALLOCATION::ALLOC_COMMIT))
			{
				output_struct.instruction.src1.value = Register_File[output_struct.instruction.src1.tag].value;
			}

			//check ROB and forwarding buses
			//if tag matches what we are looking for, and that instruction is the most recent to update 
			//the register, then grab that value, otherwise, stall.
			else
			{
				//get ROB entry location
				output_struct.instruction.src1.rob_loc = Front_End_RAT[output_struct.instruction.src1.archreg].reg;

				//check ROB
				//if ROB entry is complete, get out data
				//get ROB entry # from FE RAT ROB location entry based on arch register from src1 input
				if (ROB.entries[Front_End_RAT[output_struct.instruction.src1.archreg].rob_loc].alloc == Global::ROB_ALLOCATION::COMPLETE)
				{
					output_struct.instruction.src1.value = ROB.entries[Front_End_RAT[output_struct.instruction.src1.archreg].rob_loc].result;
				}
				//if the entry is not complete, but it is executing or waiting, check forwarding bus
				else
				{
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
					else if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_info.tag == output_struct.instruction.src1.tag)
					{
						output_struct.instruction.src1.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_info.value;
					}
					else
					{
						output_struct.instruction.src1.status = Global::STATUS::INVALID;
					}
				}
			}
		}

		if (output_struct.instruction.src2.status == Global::STATUS::INVALID)
		{
			output_struct.instruction.src2.status = Global::STATUS::VALID;

			if ((Front_End_RAT[output_struct.instruction.src2.archreg].src_bit == Global::SOURCES::REGISTER_FILE)
				&& (Register_File[Front_End_RAT[output_struct.instruction.src2.archreg].reg].status == Global::REGISTER_ALLOCATION::ALLOC_COMMIT))
			{
				output_struct.instruction.src2.value = Register_File[output_struct.instruction.src2.tag].value;
			}

			//check ROB and forwarding buses
			//if tag matches what we are looking for, and that instruction is the most recent to update 
			//the register, then grab that value, otherwise, stall.
			else
			{
				//get ROB entry location
				output_struct.instruction.src2.rob_loc = Front_End_RAT[output_struct.instruction.src2.archreg].reg;

				//check ROB
				//if ROB entry is complete, get out data
				//get ROB entry # from FE RAT ROB location entry based on arch register from src1 input
				if (ROB.entries[Front_End_RAT[output_struct.instruction.src2.archreg].rob_loc].alloc == Global::ROB_ALLOCATION::COMPLETE)
				{
					output_struct.instruction.src2.value = ROB.entries[Front_End_RAT[output_struct.instruction.src2.archreg].rob_loc].result;
				}
				//if the entry is not complete, but it is executing or waiting, check forwarding bus
				else
				{
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
					else if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_info.tag == output_struct.instruction.src2.tag)
					{
						output_struct.instruction.src2.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_info.value;
					}
					else
					{
						output_struct.instruction.src2.status = Global::STATUS::INVALID;
					}
				}
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