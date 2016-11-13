#include "stdafx.h"
#include "ALU2.h"

using namespace std;

ALU2::ALU2()
{
}


ALU2::~ALU2()
{
}

Global::apexStruct ALU2::run(Global::apexStruct input_struct, Global::Register_Info * forward_bus, bool * flags)
{
	Global::apexStruct output_struct = input_struct;
	snapshot_before = input_struct;

	//initialize flags
	flags[Global::FLAGS::OVER_FLOW] = false;
	flags[Global::FLAGS::UNDER_FLOW] = false;
	flags[Global::FLAGS::ZERO] = false;

	switch (input_struct.instruction.op_code)
	{
#pragma region "ADD"
		case Global::OPCODE::ADD:
			/* overflow */;
			if ((output_struct.instruction.src2_value > 0)
				&& (input_struct.instruction.src1_value > INT_MAX - input_struct.instruction.src2_value))
			{
				flags[Global::FLAGS::OVER_FLOW] = true;
				break;
			}
			/* underflow */;
			if ((output_struct.instruction.src2_value < 0)
				&& (input_struct.instruction.src1_value < INT_MIN - input_struct.instruction.src2_value))
			{
				flags[Global::FLAGS::UNDER_FLOW] = true;
				break;
			}
			output_struct.instruction.destination_value =
				input_struct.instruction.src1_value + input_struct.instruction.src2_value;

			break;
#pragma endregion

#pragma region "ADD w/ literal"
		case Global::OPCODE::ADDL:
			/* overflow */;
			if ((output_struct.instruction.literal_value > 0)
				&& (input_struct.instruction.src1_value > INT_MAX - input_struct.instruction.literal_value))
			{
				flags[Global::FLAGS::OVER_FLOW] = true;
				break;
			}
			/* underflow */;
			if ((output_struct.instruction.literal_value < 0)
				&& (input_struct.instruction.src1_value < INT_MIN - input_struct.instruction.literal_value))
			{
				flags[Global::FLAGS::UNDER_FLOW] = true;
				break;
			}
			output_struct.instruction.destination_value =
				input_struct.instruction.src1_value + input_struct.instruction.literal_value;

			break;
#pragma endregion

#pragma region "SUB"
		case Global::OPCODE::SUB:
			/* overflow */
			if (    (input_struct.instruction.src2_value < 0) 
				 && (input_struct.instruction.src1_value > INT_MAX + input_struct.instruction.src2_value))
			{
				flags[Global::FLAGS::OVER_FLOW] = true;
				break;
			}

			/* underflow */
			if (    (input_struct.instruction.src2_value > 0) 
				 && (input_struct.instruction.src1_value < INT_MIN + input_struct.instruction.src2_value))
			{
				flags[Global::FLAGS::UNDER_FLOW] = true;
				break;
			}

			output_struct.instruction.destination_value =
				input_struct.instruction.src1_value - input_struct.instruction.src2_value;

			break;
#pragma endregion

#pragma region "SUB w/ lieral"
		case Global::OPCODE::SUBL:
			/* overflow */
			if ((input_struct.instruction.literal_value < 0)
				&& (input_struct.instruction.src1_value > INT_MAX + input_struct.instruction.literal_value))
			{
				flags[Global::FLAGS::OVER_FLOW] = true;
				break;
			}

			/* underflow */
			if ((input_struct.instruction.literal_value > 0)
				&& (input_struct.instruction.src1_value < INT_MIN + input_struct.instruction.literal_value))
			{
				flags[Global::FLAGS::UNDER_FLOW] = true;
				break;
			}

			output_struct.instruction.destination_value =
				input_struct.instruction.src1_value - input_struct.instruction.literal_value;

			break;
#pragma endregion 

#pragma region "MUL"
		case Global::OPCODE::MUL:
			/* overflow */
			if ((input_struct.instruction.src1_value > INT_MAX / input_struct.instruction.src2_value)
				|| ((input_struct.instruction.src1_value == -1) && (input_struct.instruction.src2_value == INT_MIN)) //2's comp
				|| ((input_struct.instruction.src2_value == -1) && (input_struct.instruction.src1_value == INT_MIN))) //2' comp
			{
				flags[Global::FLAGS::OVER_FLOW] = true;
				break;
			}
			/* underflow */
			if (input_struct.instruction.src1_value < INT_MIN / input_struct.instruction.src2_value)
			{
				flags[Global::FLAGS::UNDER_FLOW] = true;
				break;
			}

			output_struct.instruction.destination_value =
				input_struct.instruction.src1_value * input_struct.instruction.src2_value;

			break;
#pragma endregion

#pragma region "MUL w/ literal"
		case Global::OPCODE::MULL:
			/* overflow */
			if ((input_struct.instruction.src1_value > INT_MAX / input_struct.instruction.literal_value)
				|| ((input_struct.instruction.src1_value == -1) && (input_struct.instruction.literal_value == INT_MIN)) //2's comp
				|| ((input_struct.instruction.literal_value == -1) && (input_struct.instruction.src1_value == INT_MIN))) //2' comp
			{
				flags[Global::FLAGS::OVER_FLOW] = true;
				break;
			}
			/* underflow */
			if (input_struct.instruction.src1_value < INT_MIN / input_struct.instruction.literal_value)
			{
				flags[Global::FLAGS::UNDER_FLOW] = true;
				break;
			}

			output_struct.instruction.destination_value =
				input_struct.instruction.src1_value * input_struct.instruction.literal_value;

			break;
#pragma endregion

#pragma region"ORs"
		case Global::OPCODE::OR:
			output_struct.instruction.destination_value = 
				input_struct.instruction.src1_value | input_struct.instruction.src2_value;
			break;
		case Global::OPCODE::ORL:
			output_struct.instruction.destination_value =
				input_struct.instruction.src1_value | input_struct.instruction.literal_value;
			break;
#pragma endregion
#pragma region "ANDs"
		case Global::OPCODE::AND:
			output_struct.instruction.destination_value =
				input_struct.instruction.src1_value & input_struct.instruction.src2_value;
			break;
		case Global::OPCODE::ANDL:
			output_struct.instruction.destination_value =
				input_struct.instruction.src1_value & input_struct.instruction.literal_value;
			break;
#pragma endregion
#pragma region "XORs"
		case Global::OPCODE::EX_OR:
			output_struct.instruction.destination_value =
				input_struct.instruction.src1_value ^ input_struct.instruction.src2_value;
			break;
		case Global::OPCODE::EX_ORL:
			output_struct.instruction.destination_value =
				input_struct.instruction.src1_value ^ input_struct.instruction.literal_value;
			break;
#pragma endregion

#pragma region "OTHERS"
		case Global::OPCODE::LOAD:
		case Global::OPCODE::STORE:
			output_struct.instruction.memory_location =
				input_struct.instruction.src1_value + input_struct.instruction.literal_value;
			break;
		case Global::OPCODE::MOVC:
			output_struct.instruction.destination_value = input_struct.instruction.literal_value;
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
			if (!flags[Global::FLAGS::OVER_FLOW]
				&& !flags[Global::FLAGS::UNDER_FLOW])
			{
				output_struct.instruction.destination_staus = Global::STATUS::VALID;
				forward_bus[Global::FORWARD_TYPE::FROM_ALU2].reg_tag = output_struct.instruction.destination_tag;
				forward_bus[Global::FORWARD_TYPE::FROM_ALU2].reg_value = output_struct.instruction.destination_value;
			}

			break;

		//No need to forward anything from the LOAD or STORE instruction at this point
		case Global::OPCODE::LOAD:
		case Global::OPCODE::STORE:
		default:
			break;
	}

	if (output_struct.instruction.destination_value == 0)
	{
		flags[Global::FLAGS::ZERO] = true;
	}
	else
	{
		flags[Global::FLAGS::ZERO] = false;
	}

	if (!flags[Global::FLAGS::OVER_FLOW]
		&& !flags[Global::FLAGS::UNDER_FLOW]
		&& !flags[Global::FLAGS::ZERO])
	{
		flags[Global::FLAGS::CLEAR] = true;
	}
	else
	{
		flags[Global::FLAGS::CLEAR] = false;
	}

	snapshot_after = output_struct;
	return output_struct;
}

void ALU2::display()
{
	Global::Debug("\n--- ALU1 stage display ---\n - ENTERING STAGE -");
	Global::Debug("pc                  : " + snapshot_before.pc_value);
	Global::Debug("op code             : " + Global::toString(snapshot_before.instruction.op_code));
	Global::Debug("destination reg tag : R" + Global::toString(snapshot_before.instruction.destination_tag));
	Global::Debug("destination value   : not ready");
	Global::Debug("source 1 reg tag    : " + Global::toString(snapshot_before.instruction.src1_tag));
	Global::Debug("source 1 reg valid  : " + Global::toString(snapshot_before.instruction.src1_valid));
	Global::Debug("source 1 reg value  : " + snapshot_before.instruction.src1_value);
	Global::Debug("source 2 reg tag    : " + Global::toString(snapshot_before.instruction.src2_tag));
	Global::Debug("source 2 reg valid  : " + Global::toString(snapshot_before.instruction.src2_valid));
	Global::Debug("source 2 reg value  : " + snapshot_before.instruction.src2_value);
	Global::Debug("literal             : " + snapshot_before.instruction.literal_value);
	Global::Debug(".....................");
	Global::Debug(" - EXITING STAGE -");
	Global::Debug("pc                  : " + snapshot_after.pc_value);
	Global::Debug("op code             : " + Global::toString(snapshot_after.instruction.op_code));
	Global::Debug("destination reg tag : " + Global::toString(snapshot_after.instruction.destination_tag));
	Global::Debug("destination value   : " + snapshot_after.instruction.destination_value);
	Global::Debug("source 1 reg tag    : " + Global::toString(snapshot_after.instruction.src1_tag));
	Global::Debug("source 1 reg valid  : " + Global::toString(snapshot_after.instruction.src1_valid));
	Global::Debug("source 1 reg value  : " + snapshot_after.instruction.src1_value);
	Global::Debug("source 2 reg tag    : " + Global::toString(snapshot_after.instruction.src2_tag));
	Global::Debug("source 2 reg valid  : " + Global::toString(snapshot_after.instruction.src2_valid));
	Global::Debug("source 2 reg value  : " + snapshot_after.instruction.src2_value);
	Global::Debug("literal             : " + snapshot_after.instruction.literal_value);
	Global::Debug("--- END ALU1 stage display ---");
}