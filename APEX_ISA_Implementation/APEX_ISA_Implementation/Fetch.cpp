#include "stdafx.h"
#include "Fetch.h"

using namespace std;

Fetch::Fetch()
{
}


Fetch::~Fetch()
{
}

ifstream& Fetch::GotoLine(ifstream& file, unsigned int num){
	file.seekg(std::ios::beg);
	for (int i = 0; i < num; ++i){
		file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
	return file;
}

Global::apexStruct Fetch::run(int (&PC), 
	Global::Forwarding_Info(&Forward_Bus)[Global::FORWARDING_BUSES], 
	bool(&Stalled_Stages)[Global::TOTAL_STAGES])
{
	unsigned int ourPC;
	Stalled_Stages[Global::STALLED_STAGE::FETCH] = false;

	if (Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].updatePC)
	{
		//for every other branch but jump, read out target and add it to PC
		if (Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].opcode != Global::OPCODE::JUMP)
		{
			Global::Debug(" -- BRANCH TAKEN! -- ");
			PC = Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].pc_value + (Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].target / 4);
			Global::Debug(" -- New PC = " + to_string(4000 + ((PC - 4000) * 4)) + " -- ");
		}
		//for jump, pc value = x + literal, which is stored in target of jump
		else
		{
			Global::Debug(" -- BRANCH TAKEN! -- ");
			PC = Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].target;
			Global::Debug(" -- New PC = " + to_string(4000 + ((PC - 4000) * 4)) + " -- ");
		}
	}
	
	ourPC = PC - 4000;
	
	Global::apexStruct output_struct;

	if (!input_file.eof())
	{
		//read out line based on PC value
		output_struct.pc_value = PC;
		if (ourPC > 0)
		{
			GotoLine(input_file, ourPC);
		}
		getline(input_file, output_struct.untouched_instruction);
	}
	
	//return updated structure
	snapshot_after = output_struct;
	return output_struct;
}

void Fetch::setFile(string filename)
{
	input_file.open(filename);

	if (!input_file)
	{
		cout << "Failed to open file: " + filename << endl;
	}
	else
	{
		cout << "File " + filename + " opened successfully." << endl;
	}
}

bool Fetch::endOfFile()
{
	return input_file.eof();
}

void Fetch::display()
{
	Global::Debug("\n--- Fetch stage display ---\n - ENTERING STAGE -");
	Global::Debug("pc                  : " + to_string(4000 + ((snapshot_after.pc_value - 4000) * 4)));
	Global::Debug("op code             : N/A");
	Global::Debug("destination reg tag : N/A");
	Global::Debug("destination value   : N/A");
	Global::Debug("source 1 reg tag    : N/A");
	Global::Debug("source 1 reg valid  : N/A");
	Global::Debug("source 1 reg value  : N/A");
	Global::Debug("source 2 reg tag    : N/A");
	Global::Debug("source 2 reg valid  : N/A");
	Global::Debug("source 2 reg value  : N/A");
	Global::Debug("literal             : N/A");
	Global::Debug(".....................");
	Global::Debug(" - EXITING STAGE -");
	Global::Debug("pc                  : " + to_string(4000 + ((snapshot_after.pc_value - 4000) * 4)));
	Global::Debug("raw instruction     : " + snapshot_after.untouched_instruction);
	Global::Debug("op code             : N/A");
	Global::Debug("destination reg tag : N/A");
	Global::Debug("destination value   : N/A");
	Global::Debug("source 1 reg tag    : N/A");
	Global::Debug("source 1 reg valid  : N/A");
	Global::Debug("source 1 reg value  : N/A");
	Global::Debug("source 2 reg tag    : N/A");
	Global::Debug("source 2 reg valid  : N/A");
	Global::Debug("source 2 reg value  : N/A");
	Global::Debug("literal             : N/A");
	Global::Debug("--- END FETCH stage display ---");
}