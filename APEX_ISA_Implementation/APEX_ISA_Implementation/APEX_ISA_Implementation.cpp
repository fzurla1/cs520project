/* Eric Garbade & Frank Zurla
 * CS520
 * Fall 2016
 * Professor Kanad Ghose
 * APEX (A Pipeline EXample) emulator Project 1
 */
// APEX_ISA_Implementation.cpp : Defines the entry point for the console application.
//

/* APEX is an instruction pipeline that implements a load-store instruction set.
 */

#include "APEX_ISA_Implementation.h"
#include "stdafx.h"
#include <stdio.h>
#include <fstream>
#include <string>
#include <iostream>
#include <ctime>
#include <algorithm>

#include "Global.h"
#include "Fetch.h"
#include "Decode.h"
#include "ALU1.h"
#include "ALU2.h"
#include "Branch.h"
#include "Delay.h"
#include "Memory.h"
#include "WriteBack.h"

using namespace std;

//--------VARIABLES--------//

//program counter
int PC = 4000;

//flags from ALU
bool alu_flags[Global::ALU_FLAG_COUNT];

//Register file
Global::Register_Info register_file[Global::ARCH_REGISTER_COUNT];

//forwarding bus
Global::Register_Info forward_bus[Global::FORWARDING_BUSES];

bool alu1_stalled = false,
branch_stalled = false,
memory_stalled = false;

//Memory - 4 bytes wide, 0 to 3999
int memory_array[Global::MEMORY_SIZE];

int most_recent_reg[Global::ARCH_REGISTER_COUNT];

//-----------FUNCTIONS---------//

#pragma region "INITIALIZE FUNCTIONS" 
void initialize_flags()
{
	for (int x = 0; x < Global::ALU_FLAG_COUNT; x++)
	{
		alu_flags[x] = false;
	}
}

void initialize_forwarding_bus()
{
	for (int x = 0; x < Global::FORWARDING_BUSES; x++)
	{
		forward_bus[x].clear();
	}
}

void initialize_memory()
{
	for each (int x in memory_array)
	{
		memory_array[x] = 0;
	}
}

void initialize_register_file()
{
	for (int x = 0; x < Global::ARCH_REGISTER_COUNT; x++)
	{
		register_file[x].clear();
	}
}

void initialize_most_recent()
{
	for each(int x in most_recent_reg)
	{
		most_recent_reg[x] = Global::ARCH_REGISTERS::NA;
	}
}

void initialize_pipeline()
{
	cout << "Initializing Flags" << endl
		<< "..." << endl;
	initialize_flags();
	cout << "Initializing Flags Complete" << endl;
	cout << "Initializing Forwarding Bus" << endl
		<< "..." << endl;
	initialize_forwarding_bus();
	cout << "Initializing Forwarding Bus Complete" << endl;
	cout << "Initializing Register File" << endl
		<< "..." << endl;
	initialize_register_file();
	cout << "Initializing Register File Complete" << endl;
	cout << "Initializing Memory" << endl
		<< "..." << endl;
	initialize_memory();
	cout << "Initializing Memory Complete" << endl;
	cout << "Resetting History" << endl
		<< "..." << endl;
	initialize_most_recent();
	cout << "Resetting History Complete" << endl;
	cout << "Resetting PC" << endl
		<< "..." << endl;
	PC = 4000;
	cout << "Resetting PC Complete" << endl;
}
#pragma endregion

string trim(string &str)
{
	size_t first = str.find_first_not_of(' ');
	size_t last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
}

void user_interface(char * input_file, char * output_file)
{
	string infile_name = "", 
		outfile_name = "";

	cout << "Welcome to the APEX ISA Implementation for CS520" << endl
		<< "Project 1" << endl
		<< "Prof Ghose" << endl
		<< "Fall 2016" << endl
		<< "Frank Zurla and Eric Garbade" << endl
		<< endl
		<< "Usage: project_1 " << endl
		<< "  options: <input file>  = APEX instruction set to run" << endl
		<< "         : <output_file> = output file to write to" << endl
		<< "                         = default is apex_output[date_time].txt" << endl
		<< endl;

	if (input_file != NULL)
	{
		cout << "input file = " << input_file << endl;
	}
	else
	{
		while (infile_name == "")
		{
			cout << "please enter input instruction file: ";
			getline(cin, infile_name);
		}
		input_file = new char[infile_name.length() + 1];
		strcpy_s(input_file, trim(infile_name).length(), trim(infile_name).c_str());
	}

	if (output_file != NULL)
	{
		cout << endl << "output file = " << output_file << endl;
		Global::setOutFile(output_file);
	}
	else
	{
		cout << "please enter output file (or blank): ";
		getline(cin, outfile_name);

		if (outfile_name == "")
		{
			cout << endl << "using default output file: apex_output_date_time.txt" << endl;
			outfile_name = "apex_output[";
			
			// current date/time based on current system
			time_t now = time(0);
			tm ltm;
			localtime_s(&ltm, &now);

			// print various components of tm structure.
			outfile_name.append(to_string(1 + ltm.tm_mon) + to_string(ltm.tm_mday) + to_string(1900 + ltm.tm_year));
			outfile_name.append("_" + to_string(ltm.tm_hour) + "-" + to_string(ltm.tm_min) + "-" + to_string(ltm.tm_sec) + "].txt");
			cout << "File name = " + outfile_name << endl;
		}
		else
		{
			outfile_name = trim(outfile_name);
		}

		output_file = new char[outfile_name.length() + 1];
		strcpy_s(output_file, trim(outfile_name).length(), trim(outfile_name).c_str());
		Global::setOutFile(outfile_name);
	}
}

//MAIN PROGRAM
int _tmain(int argc, char* argv[])
{
#pragma region LOCAL VARIABLES
	Fetch * fetch = new Fetch();
	Decode * decode = new Decode();
	ALU1 * alu1 = new ALU1();
	ALU2 * alu2 = new ALU2();
	Branch * branch = new Branch();
	Delay * delay = new Delay();
	Memory * memory = new Memory();
	WriteBack * writeBack = new WriteBack();
	ifstream input_file;
	ofstream output_file;
	char * input_file_name = NULL;
	char * output_file_name = NULL;
	string command = "",
		tempS = "";

	int HALT = 0,
		n = 0;

	Global::apexStruct pipeline_struct_fetch;
	Global::apexStruct pipeline_struct_decode;
	Global::apexStruct pipeline_struct_alu1;
	Global::apexStruct pipeline_struct_alu2;
	Global::apexStruct pipeline_struct_branch;
	Global::apexStruct pipeline_struct_delay;
	Global::apexStruct pipeline_struct_memory;
#pragma endregion //LOCAL VARIABLES

	input_file_name = argv[0];
	output_file_name = argv[1];

	user_interface(input_file_name, output_file_name);
	input_file.open(input_file_name);

	if (input_file)
	{
		while (command != "end" && !input_file.eof())
		{

			cout << "Available commands:" << endl
				<< "   initialize   : initialize pipeline" << endl
				<< "   simulate <n> : simulate n number of pipeline steps" << endl
				<< "   display      : display contents of the pipeline" << endl
				<< "   commands     : display command list" << endl
				<< "   end          : stop execution" << endl
				<< endl;
			getline(cin, command);
			command = trim(command);
			std::transform(command.begin(), command.end(), command.begin(), ::tolower);

#pragma region BASIC COMMANDS

			if (command == "initialize")
			{
				initialize_pipeline();
			}

			if (command == "commands")
			{
				cout << "Available commands:" << endl
					<< "   initialize   : initialize pipeline" << endl
					<< "   simulate <n> : simulate n number of pipeline steps" << endl
					<< "   display      : display contents of the pipeline" << endl
					<< "   commands     : display command list" << endl
					<< "   end          : stop execution" << endl
					<< endl;
			}

			if (command == "display")
			{
				cout << "Writing pipeline stages..." << endl;
				Global::Debug("Writing pipeline stages...");
				fetch->display();
				cout << "." << endl;
				decode->display();
				cout << "." << endl;
				alu1->display();
				cout << "." << endl;
				alu2->display();
				cout << "." << endl;
				branch->display();
				cout << "." << endl;
				delay->display();
				cout << "." << endl;
				memory->display();
				cout << "." << endl;
				writeBack->display();
				cout << "Writing pipeline stages Complete" << endl;
			}

#pragma endregion //BASIC COMMANDS

#pragma region SIMULATION

			if (command.compare(0, 7, "simulate") == 0) //command is simulate
			{
				//get number of iterations to simulate
				tempS = command.substr(9, command.length() - 8);
				n = atoi(tempS.c_str());

				while (!HALT && n > 0)
				{
					//start pipeline
					pipeline_struct_fetch = fetch->run(PC);
					pipeline_struct_decode = decode->run(pipeline_struct_fetch, register_file);

					if (!HALT) //HALT has not occured
					{
						pipeline_struct_alu1 = alu1->run(pipeline_struct_decode, forward_bus);
						if (!alu1->isStalled())
						{
							pipeline_struct_alu2 = alu2->run(pipeline_struct_alu1, forward_bus, alu_flags);
						}
						pipeline_struct_branch = branch->run(pipeline_struct_decode, forward_bus, alu_flags);
						if (!branch->isStalled())
						{
							pipeline_struct_delay = delay->run(pipeline_struct_branch);
						}
						if (pipeline_struct_alu2.pc_value < pipeline_struct_delay.pc_value)
						{
							pipeline_struct_memory = memory->run(pipeline_struct_alu2, forward_bus, memory_array);
						}
						else
						{
							pipeline_struct_memory = memory->run(pipeline_struct_delay, forward_bus, memory_array);
						}
						writeBack->run(pipeline_struct_memory, forward_bus, register_file, &HALT);
					} // !HALT

					pipeline_struct_fetch.clear();
					pipeline_struct_decode.clear();
					pipeline_struct_alu1.clear();
					pipeline_struct_alu2.clear();
					pipeline_struct_branch.clear();
					pipeline_struct_delay.clear();
					pipeline_struct_memory.clear();
					n--;
				} //!HALT && n > 0
			}
		} //command != "end"
	}
#pragma endregion //SIMULATION

	else
	{
		cout << "Error with input file: " << input_file_name << endl
			<< "EXITING" << endl;
	}

	//destruct
	fetch->~Fetch();
	decode->~Decode();
	alu1->~ALU1();
	alu2->~ALU2();
	branch->~Branch();
	delay->~Delay();
	memory->~Memory();
	writeBack->~WriteBack();

	input_file.close();
	Global::closeFile();

	//delete & free up memory
	delete fetch;
	delete decode;
	delete alu1;
	delete alu2;
	delete branch;
	delete delay;
	delete memory;
	delete writeBack;

	return 0;
}