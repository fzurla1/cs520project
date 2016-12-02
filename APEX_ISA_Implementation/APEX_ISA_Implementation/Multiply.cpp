#include "stdafx.h"
#include "Multiply.h"


Multiply::Multiply()
{
}


Multiply::~Multiply()
{
}


Global::apexStruct Multiply::run(Global::Forwarding_Info(&Forward_Bus)[Global::FINAL_FORWARD_TYPE_TOTAL],
	Global::Register_Info *Register_File,
	Global::Reorder_Buffer(&ROB))
{
	Global::apexStruct output_struct = myStruct;
	snapshot_before = myStruct;
	
	//make sure we have valid data
	if (myStruct.pc_value != INT_MAX)
	{
		//takes 4 cycles to complete
		if (count == 4)
		{
			switch (myStruct.instruction.op_code)
			{
#pragma region "MUL"
				case Global::OPCODE::MUL:

					output_struct.instruction.dest.value =
						myStruct.instruction.src1.value * myStruct.instruction.src2.value;

					break;
#pragma endregion

#pragma region "MUL w/ literal"
				case Global::OPCODE::MULL:

					output_struct.instruction.dest.value =
						myStruct.instruction.src1.value * myStruct.instruction.literal_value;

					break;
#pragma endregion
				default:
					break;
			}

			//mark as valid
			output_struct.instruction.dest.status = Global::STATUS::VALID;

			//update Forward Bus
			Forward_Bus[Global::FORWARD_TYPE::FROM_MULTIPLY].pc_value = output_struct.pc_value;
			Forward_Bus[Global::FORWARD_TYPE::FROM_MULTIPLY].reg_info.tag = output_struct.instruction.dest.tag;
			Forward_Bus[Global::FORWARD_TYPE::FROM_MULTIPLY].reg_info.value = output_struct.instruction.dest.value;

			//Update register file
			Register_File[output_struct.instruction.dest.tag].status = Global::REGISTER_ALLOCATION::ALLOC_NO_COMMIT;
			Register_File[output_struct.instruction.dest.tag].value = output_struct.instruction.dest.value;

			//Update ROB
			ROB.entries[output_struct.instruction.dest.rob_loc].alloc = Global::ROB_ALLOCATION::COMPLETE;
			ROB.entries[output_struct.instruction.dest.rob_loc].flags = output_struct.instruction.flag;
			ROB.entries[output_struct.instruction.dest.rob_loc].result = output_struct.instruction.dest.value;

			//reset count
			count = 1;
		}
		else
		{
			count++;
		}
	}

	return output_struct;
}


void Multiply::setPipelineStruct(Global::apexStruct input_struct)
{
	myStruct = input_struct;
}


bool Multiply::hasValidData()
{
	return (myStruct.pc_value != INT_MAX);
}


string Multiply::getInstruction()
{
	return myStruct.untouched_instruction;
}


void Multiply::display()
{
	//make sure we have valid data
	if (myStruct.pc_value != INT_MAX)
	{
		Global::Debug("MULTIPLY  - " + snapshot_before.untouched_instruction);
	}
	else
	{
		Global::Debug("MULTIPLY 2 STAGE --> No Instruction in Stage");
	}
}