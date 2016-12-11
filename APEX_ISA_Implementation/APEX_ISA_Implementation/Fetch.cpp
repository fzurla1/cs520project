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
	for (unsigned int i = 0; i < num; ++i){
		file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
	return file;
}

Global::apexStruct Fetch::run(int (&PC), 
	Global::Forwarding_Info(&Forward_Bus)[Global::FINAL_FORWARD_TYPE_TOTAL], 
	bool(&Stalled_Stages)[Global::FINAL_STALLED_STAGE_TOTAL])
{
	int ourPC = 0;
	Stalled_Stages[Global::STALLED_STAGE::FETCH] = false;

	if (Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].updatePC)
	{
		//for every other branch but jump, read out target and add it to PC
		if (Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].opcode != Global::OPCODE::JUMP)
		{
			Global::Output(" -- BRANCH TAKEN! -- ");
			PC = Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].pc_value + (Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].target / 4);
			Global::Output(" -- New PC = " + to_string(4000 + ((PC - 4000) * 4)) + " -- ");
			input_file.seekg(std::ios::beg);
		}
		//for jump, pc value = x + literal, which is stored in target of jump
		else
		{
			Global::Output(" -- BRANCH TAKEN! -- ");
			PC = Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].target;
			Global::Output(" -- New PC = " + to_string(4000 + ((PC - 4000) * 4)) + " -- ");
			input_file.seekg(std::ios::beg);
		}
	}
	
	ourPC = PC - 4000;
	if (ourPC == 0)
	{
		input_file.seekg(std::ios::beg);
	}
	
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

bool Fetch::fileIsGood()
{
	return input_file.is_open() & input_file.good();
}

bool Fetch::endOfFile()
{
	return input_file.eof();
}

void Fetch::closeFile()
{
	input_file.close();
}

void Fetch::display()
{
	Global::Output("FETCH      - " + snapshot_after.untouched_instruction);
}