#include "stdafx.h"
#include "ALU2.h"

using namespace std;

extern int Stalled_Stages;
extern bool ALU_Flags;
extern Global::Register_Info Forward_Bus;

ALU2::ALU2()
{
}


ALU2::~ALU2()
{
}

Global::apexStruct ALU2::run(
	bool(&ALU_Flags)[Global::ALU_FLAG_COUNT],
	Global::Forwarding_Info(&Forward_Bus)[Global::FORWARDING_BUSES],
	bool(&Stalled_Stages)[Global::TOTAL_STAGES])
{
	Global::apexStruct output_struct = myStruct;
	snapshot_before = myStruct;

	//initialize flags
	Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].flag = Global::FLAGS::CLEAR;
	/*
	ALU_Flags[Global::FLAGS::ZERO] = true;
	ALU_Flags[Global::FLAGS::OVER_FLOW] = false;
	ALU_Flags[Global::FLAGS::UNDER_FLOW] = false;
	ALU_Flags[Global::FLAGS::ZERO] = false;
	*/

	//make sure we have valid data
	if (myStruct.pc_value != INT_MAX)
	{
		if (!Stalled_Stages[Global::STALLED_STAGE::MEMORY])
		{
			//assume we are not stalled
			Stalled_Stages[Global::STALLED_STAGE::ALU2] = false;

			switch (myStruct.instruction.op_code)
			{
#pragma region "ADD"
			case Global::OPCODE::ADD:
				/* overflow */;
				if ((output_struct.instruction.src2.value > 0)
					&& (myStruct.instruction.src1.value > INT_MAX - myStruct.instruction.src2.value))
				{
					ALU_Flags[Global::FLAGS::OVER_FLOW] = true;
					Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].flag = Global::FLAGS::OVER_FLOW;
				}
				/* underflow */;
				if ((output_struct.instruction.src2.value < 0)
					&& (myStruct.instruction.src1.value < INT_MIN - myStruct.instruction.src2.value))
				{
					ALU_Flags[Global::FLAGS::UNDER_FLOW] = true;
					Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].flag = Global::FLAGS::UNDER_FLOW;
				}
				output_struct.instruction.dest.value =
					myStruct.instruction.src1.value + myStruct.instruction.src2.value;

				break;
#pragma endregion

#pragma region "ADD w/ literal"
			case Global::OPCODE::ADDL:
				/* overflow */;
				if ((output_struct.instruction.literal_value > 0)
					&& (myStruct.instruction.src1.value > INT_MAX - myStruct.instruction.literal_value))
				{
					ALU_Flags[Global::FLAGS::OVER_FLOW] = true;
					Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].flag = Global::FLAGS::OVER_FLOW;
				}
				/* underflow */;
				if ((output_struct.instruction.literal_value < 0)
					&& (myStruct.instruction.src1.value < INT_MIN - myStruct.instruction.literal_value))
				{
					ALU_Flags[Global::FLAGS::UNDER_FLOW] = true;
					Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].flag = Global::FLAGS::UNDER_FLOW;
				}
				output_struct.instruction.dest.value =
					myStruct.instruction.src1.value + myStruct.instruction.literal_value;

				break;
#pragma endregion

#pragma region "SUB"
			case Global::OPCODE::SUB:
				/* overflow */
				if ((myStruct.instruction.src2.value < 0)
					&& (myStruct.instruction.src1.value > INT_MAX + myStruct.instruction.src2.value))
				{
					ALU_Flags[Global::FLAGS::OVER_FLOW] = true;
					Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].flag = Global::FLAGS::OVER_FLOW;
				}

				/* underflow */
				if ((myStruct.instruction.src2.value > 0)
					&& (myStruct.instruction.src1.value < INT_MIN + myStruct.instruction.src2.value))
				{
					ALU_Flags[Global::FLAGS::UNDER_FLOW] = true;
					Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].flag = Global::FLAGS::UNDER_FLOW;
				}

				output_struct.instruction.dest.value =
					myStruct.instruction.src1.value - myStruct.instruction.src2.value;

				break;
#pragma endregion

#pragma region "SUB w/ lieral"
			case Global::OPCODE::SUBL:
				/* overflow */
				if ((myStruct.instruction.literal_value < 0)
					&& (myStruct.instruction.src1.value > INT_MAX + myStruct.instruction.literal_value))
				{
					ALU_Flags[Global::FLAGS::OVER_FLOW] = true;
					Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].flag = Global::FLAGS::OVER_FLOW;
				}

				/* underflow */
				if ((myStruct.instruction.literal_value > 0)
					&& (myStruct.instruction.src1.value < INT_MIN + myStruct.instruction.literal_value))
				{
					ALU_Flags[Global::FLAGS::UNDER_FLOW] = true;
					Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].flag = Global::FLAGS::UNDER_FLOW;
				}

				output_struct.instruction.dest.value =
					myStruct.instruction.src1.value - myStruct.instruction.literal_value;

				break;
#pragma endregion 

#pragma region "MUL"
			case Global::OPCODE::MUL:
				/* overflow */
				if ((myStruct.instruction.src1.value > INT_MAX / myStruct.instruction.src2.value)
					|| ((myStruct.instruction.src1.value == -1) && (myStruct.instruction.src2.value == INT_MIN)) //2's comp
					|| ((myStruct.instruction.src2.value == -1) && (myStruct.instruction.src1.value == INT_MIN))) //2' comp
				{
					ALU_Flags[Global::FLAGS::OVER_FLOW] = true;
					Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].flag = Global::FLAGS::OVER_FLOW;
				}
				/* underflow */
				if (myStruct.instruction.src1.value < INT_MIN / myStruct.instruction.src2.value)
				{
					ALU_Flags[Global::FLAGS::UNDER_FLOW] = true;
					Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].flag = Global::FLAGS::UNDER_FLOW;
				}

				output_struct.instruction.dest.value =
					myStruct.instruction.src1.value * myStruct.instruction.src2.value;

				break;
#pragma endregion

#pragma region "MUL w/ literal"
			case Global::OPCODE::MULL:
				/* overflow */
				if ((myStruct.instruction.src1.value > INT_MAX / myStruct.instruction.literal_value)
					|| ((myStruct.instruction.src1.value == -1) && (myStruct.instruction.literal_value == INT_MIN)) //2's comp
					|| ((myStruct.instruction.literal_value == -1) && (myStruct.instruction.src1.value == INT_MIN))) //2' comp
				{
					ALU_Flags[Global::FLAGS::OVER_FLOW] = true;
					Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].flag = Global::FLAGS::OVER_FLOW;
				}
				/* underflow */
				if (myStruct.instruction.src1.value < INT_MIN / myStruct.instruction.literal_value)
				{
					ALU_Flags[Global::FLAGS::UNDER_FLOW] = true;
					Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].flag = Global::FLAGS::UNDER_FLOW;
				}

				output_struct.instruction.dest.value =
					myStruct.instruction.src1.value * myStruct.instruction.literal_value;

				break;
#pragma endregion

#pragma region"ORs"
			case Global::OPCODE::OR:
				output_struct.instruction.dest.value =
					myStruct.instruction.src1.value | myStruct.instruction.src2.value;
				break;
			case Global::OPCODE::ORL:
				output_struct.instruction.dest.value =
					myStruct.instruction.src1.value | myStruct.instruction.literal_value;
				break;
#pragma endregion

#pragma region "ANDs"
			case Global::OPCODE::AND:
				output_struct.instruction.dest.value =
					myStruct.instruction.src1.value & myStruct.instruction.src2.value;
				break;
			case Global::OPCODE::ANDL:
				output_struct.instruction.dest.value =
					myStruct.instruction.src1.value & myStruct.instruction.literal_value;
				break;
#pragma endregion

#pragma region "XORs"
			case Global::OPCODE::EX_OR:
				output_struct.instruction.dest.value =
					myStruct.instruction.src1.value ^ myStruct.instruction.src2.value;
				break;
			case Global::OPCODE::EX_ORL:
				output_struct.instruction.dest.value =
					myStruct.instruction.src1.value ^ myStruct.instruction.literal_value;
				break;
#pragma endregion

#pragma region "OTHERS"
			case Global::OPCODE::LOAD:
				output_struct.instruction.memory_location =
					myStruct.instruction.src1.value + myStruct.instruction.literal_value;
				break;
			case Global::OPCODE::STORE:
				output_struct.instruction.memory_location =
					myStruct.instruction.src2.value + myStruct.instruction.literal_value;
				break;
			case Global::OPCODE::MOVC:
				output_struct.instruction.dest.value = myStruct.instruction.literal_value;
				break;
#pragma endregion

			default:
				break;
			}

			//forward data
			switch (output_struct.instruction.op_code)
			{
				//forward everything except LOAD and STORE data
			case Global::OPCODE::ADD:
			case Global::OPCODE::ADDL:
			case Global::OPCODE::SUB:
			case Global::OPCODE::SUBL:
			case Global::OPCODE::MUL:
			case Global::OPCODE::MULL:
			case Global::OPCODE::OR:
			case Global::OPCODE::ORL:
			case Global::OPCODE::AND:
			case Global::OPCODE::ANDL:
			case Global::OPCODE::EX_OR:
			case Global::OPCODE::EX_ORL:
			case Global::OPCODE::MOVC:
				output_struct.instruction.dest.status = Global::STATUS::VALID;
				Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].pc_value = output_struct.pc_value;
				Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].reg_info.tag = output_struct.instruction.dest.tag;
				Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].reg_info.value = output_struct.instruction.dest.value;

				break;

				//No need to forward anything from the LOAD or STORE instruction at this point
			case Global::OPCODE::LOAD:
			case Global::OPCODE::STORE:
			default:
				break;
			}

			if (output_struct.instruction.dest.value == 0)
			{
				ALU_Flags[Global::FLAGS::ZERO] = true;
				Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].flag = Global::FLAGS::ZERO;
			}
			else
			{
				ALU_Flags[Global::FLAGS::ZERO] = false;
			}

			if (!ALU_Flags[Global::FLAGS::OVER_FLOW]
				&& !ALU_Flags[Global::FLAGS::UNDER_FLOW]
				&& !ALU_Flags[Global::FLAGS::ZERO])
			{
				ALU_Flags[Global::FLAGS::CLEAR] = true;
				Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].flag = Global::FLAGS::CLEAR;
			}
			else
			{
				ALU_Flags[Global::FLAGS::CLEAR] = false;
			}
		}
		else
		{
			Stalled_Stages[Global::STALLED_STAGE::ALU2] = true;
		}
	}

	snapshot_after = output_struct;
	return output_struct;
}

void ALU2::setPipelineStruct(Global::apexStruct input_struct)
{
	myStruct = input_struct;
}

bool ALU2::hasValidData()
{
	return (myStruct.pc_value != INT_MAX);

}
void ALU2::display()
{
	//make sure we have valid data
	if (myStruct.pc_value != INT_MAX)
	{
		Global::Debug("ALU2       - " + snapshot_before.untouched_instruction);
		/*
		Global::Debug("\n--- ALU2 stage display ---\n - ENTERING STAGE -");
		Global::Debug("pc                  : " + to_string(4000 + ((snapshot_before.pc_value - 4000) * 4)));
		Global::Debug("raw instruction     : " + snapshot_before.untouched_instruction);
		Global::Debug("op code             : " + Global::toString(snapshot_before.instruction.op_code));
		Global::Debug("destination reg tag : " + Global::toString(snapshot_before.instruction.dest.tag));
		Global::Debug("destination value   : not ready");
		Global::Debug("source 1 reg tag    : " + Global::toString(snapshot_before.instruction.src1.tag));
		Global::Debug("source 1 reg valid  : " + Global::toString(snapshot_before.instruction.src1.status));
		Global::Debug("source 1 reg value  : " + to_string(snapshot_before.instruction.src1.value));
		Global::Debug("source 2 reg tag    : " + Global::toString(snapshot_before.instruction.src2.tag));
		Global::Debug("source 2 reg valid  : " + Global::toString(snapshot_before.instruction.src2.status));
		Global::Debug("source 2 reg value  : " + to_string(snapshot_before.instruction.src2.value));
		Global::Debug("literal             : " + to_string(snapshot_before.instruction.literal_value));
		Global::Debug(".....................");
		Global::Debug(" - EXITING STAGE -");
		Global::Debug("pc                  : " + to_string(4000 + ((snapshot_after.pc_value - 4000) * 4)));
		Global::Debug("op code             : " + Global::toString(snapshot_after.instruction.op_code));
		Global::Debug("destination reg tag : " + Global::toString(snapshot_after.instruction.dest.tag));
		Global::Debug("destination value   : " + to_string(snapshot_after.instruction.dest.value));
		Global::Debug("source 1 reg tag    : " + Global::toString(snapshot_after.instruction.src1.tag));
		Global::Debug("source 1 reg valid  : " + Global::toString(snapshot_after.instruction.src1.status));
		Global::Debug("source 1 reg value  : " + to_string(snapshot_after.instruction.src1.value));
		Global::Debug("source 2 reg tag    : " + Global::toString(snapshot_after.instruction.src2.tag));
		Global::Debug("source 2 reg valid  : " + Global::toString(snapshot_after.instruction.src2.status));
		Global::Debug("source 2 reg value  : " + to_string(snapshot_after.instruction.src2.value));
		Global::Debug("literal             : " + to_string(snapshot_after.instruction.literal_value));
		Global::Debug("--- END ALU2 stage display ---");
		*/
	}
	else
	{
		Global::Debug("ALU 2 STAGE --> No Instruction in Stage");
	}
}