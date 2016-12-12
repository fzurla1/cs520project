#include "stdafx.h"
#include "IssueQueue.h"


IssueQueue::IssueQueue()
{
}


IssueQueue::~IssueQueue()
{
}

void IssueQueue::addIQEntry(std::vector<Global::apexStruct> &IQ, Global::apexStruct entry)
{
	IQ.insert(IQ.begin(),entry);
}

void IssueQueue::removeIQEntry(std::vector<Global::apexStruct> &IQ, int index)
{
	IQ.erase(IQ.begin() + index);
}

bool IssueQueue::sameOpCode(Global::apexStruct current, std::vector<Global::apexStruct> output)
{
	if (output.size() > 0)
	{
		for (int i = 0; i < output.size(); i++)
		{
			if (current.instruction.op_code == output[i].instruction.op_code)
			{
				return true;
			}
		}
	}
	return false;
}


bool IssueQueue::stalledStage(Global::apexStruct current, bool(&Stalled_Stages)[Global::STALLED_STAGE::FINAL_STALLED_STAGE_TOTAL])
{
	Global::OPCODE opc = current.instruction.op_code;

	switch (opc)
	{
	case Global::OPCODE::ADD:
	case Global::OPCODE::AND:
	case Global::OPCODE::EX_OR:
	case Global::OPCODE::OR:
	case Global::OPCODE::SUB:
	case Global::OPCODE::ADDL:
	case Global::OPCODE::ANDL:
	case Global::OPCODE::EX_ORL:
	case Global::OPCODE::ORL:
	case Global::OPCODE::SUBL:
		if (Stalled_Stages[Global::STALLED_STAGE::ALU2])
		{
			return true;
		}
	case Global::OPCODE::MUL:
	case Global::OPCODE::MULL:
		//NEED TO ADD MUL TO STALLED_STAGES
		if (Stalled_Stages[Global::STALLED_STAGE::MULTIPLY])
		{
			return true;
		}
	case Global::OPCODE::LOAD:
	case Global::OPCODE::STORE:
		//NEED TO ADD LOAD/STORE TO STALLED_STAGES
		if (Stalled_Stages[Global::STALLED_STAGE::LS])
		{
			return true;
		}
	case Global::OPCODE::BZ:
	case Global::OPCODE::BNZ:
	case Global::OPCODE::BAL:
	case Global::OPCODE::MOVC:
	case Global::OPCODE::JUMP:
		if (Stalled_Stages[Global::STALLED_STAGE::BRANCH])
		{
			return true;
		}
	}
	return false;
}

std::vector<Global::apexStruct> IssueQueue::run(Global::Forwarding_Info(&Forward_Bus)[Global::FORWARD_TYPE::FINAL_FORWARD_TYPE_TOTAL],
	bool(&Stalled_Stages)[Global::STALLED_STAGE::FINAL_STALLED_STAGE_TOTAL])
{
	//check if IQ is full and set flag if so
	if (IQ.size() >= 12)
	{
		//NEED TO ADD IQ TO STALLED_STAGE
		Stalled_Stages[Global::STALLED_STAGE::IQ] = true;
	}

	//clear output at the start
	output.clear();

	//loop through IQ to find instructions ready for dispatch, put up to 3 in output (only 1 per FU)
	if (IQ.size() > 0)
	{
		snapshotBefore = IQ;
		for (int i = IQ.size() - 1; i >= 0; i--)
		{
			if (output.size() >= 3)
			{
				break;
			}

			Global::apexStruct current = IQ[i];

			//skip to next IQ entry if the opcode exists in output already
			if (sameOpCode(current, output))
			{
				continue;
			}

			//check if FU is ready to receive instruction
			if (stalledStage(current, Stalled_Stages))
			{
				continue;
			}

			/*  FAZ -- removed this - its done twice
			//issue instruction if both sources valid
			if (current.instruction.src1.status == Global::STATUS::VALID && current.instruction.src2.status == Global::STATUS::VALID)
			{
				output.push_back(current);
				removeIQEntry(IQ, i);
			}
			*/

			//look for src1 from forward bus
			if (current.instruction.src1.status == Global::STATUS::INVALID)
			{
				//check from ALU2
				if (Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].reg_info.tag == current.instruction.src1.tag)
				{
					current.instruction.src1.value = Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].reg_info.value;
				}
				//check from memory
				else if (Forward_Bus[Global::FORWARD_TYPE::FROM_MEMORY].reg_info.tag == current.instruction.src1.tag)
				{
					current.instruction.src1.value = Forward_Bus[Global::FORWARD_TYPE::FROM_MEMORY].reg_info.value;
				}
				//check from writeback
				else if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_info.tag == current.instruction.src1.tag)
				{
					current.instruction.src1.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_info.value;
				}
				//check from multiply
				else if (Forward_Bus[Global::FORWARD_TYPE::FROM_MULTIPLY].reg_info.tag == current.instruction.src1.tag)
				{
					current.instruction.src1.value = Forward_Bus[Global::FORWARD_TYPE::FROM_MULTIPLY].reg_info.value;
				}
				//check from LS
				//NEED LS
			}
			//look for src2 from forward bus
			if (current.instruction.src2.status == Global::STATUS::INVALID)
			{
				//check from ALU2
				if (Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].reg_info.tag == current.instruction.src2.tag)
				{
					current.instruction.src2.value = Forward_Bus[Global::FORWARD_TYPE::FROM_ALU2].reg_info.value;
				}
				//check from memory
				else if (Forward_Bus[Global::FORWARD_TYPE::FROM_MEMORY].reg_info.tag == current.instruction.src2.tag)
				{
					current.instruction.src2.value = Forward_Bus[Global::FORWARD_TYPE::FROM_MEMORY].reg_info.value;
				}
				//check from writeback
				else if (Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_info.tag == current.instruction.src2.tag)
				{
					current.instruction.src2.value = Forward_Bus[Global::FORWARD_TYPE::FROM_WRITEBACK].reg_info.value;
				}
				//check from multiply
				else if (Forward_Bus[Global::FORWARD_TYPE::FROM_MULTIPLY].reg_info.tag == current.instruction.src2.tag)
				{
					current.instruction.src2.value = Forward_Bus[Global::FORWARD_TYPE::FROM_MULTIPLY].reg_info.value;
				}
				//check from LS
				//NEED LS
			}

			//after searching for src1/2, issue instruction if both sources valid
			if (current.instruction.src1.status == Global::STATUS::VALID && current.instruction.src2.status == Global::STATUS::VALID)
			{
				output.push_back(current);
				removeIQEntry(IQ, i);
			}
		}
	}

	return output;
}

void IssueQueue::setPipelineStruct(Global::apexStruct inputStruct)
{
	//add if there is a valid instruction
	if (inputStruct.pc_value != INT_MAX)
	{
		addIQEntry(IQ, inputStruct);
	}	
}

void IssueQueue::display()
{
	if (snapshotBefore.size() > 0)
	{
		Global::Output("-- Issue Queue --");
		for (int x = 0; x < snapshotBefore.size(); x++)
		{
			Global::Output(" IQ slot[" + to_string(x) + "]");
			Global::Output("     Instruction: " + snapshotBefore[x].untouched_instruction);
		}
	}
	else
	{
		Global::Output("Issue Queue --> No Instruction in Issue Queue");
	}
}