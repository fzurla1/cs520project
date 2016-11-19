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
#include <string.h>
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

#define DEBUG 1

using namespace std;

//--------VARIABLES--------//

//program counter
int PC = 4000;

//stalled stages
bool Stalled_Stages[Global::TOTAL_STAGES];

//flags from ALU
bool ALU_Flags[Global::ALU_FLAG_COUNT];

//Register file
Global::Register_Info Register_File[Global::ARCH_REGISTER_COUNT];

//forwarding bus
Global::Forwarding_Info Forward_Bus[Global::FORWARDING_BUSES];

//Memory - 4 bytes wide, 0 to 3999
int Memory_Array[Global::MEMORY_SIZE];

//contains PC values
int Most_Recent_Reg[Global::ARCH_REGISTER_COUNT];

//identify when instructions are still in the pipeline
bool pipelineHasData = true;

Fetch * fetch;
Decode * decode;
ALU1 * alu1;
ALU2 * alu2;
Branch * branch;
Delay * delay;
Memory * memory;
WriteBack * writeBack;

//-----------FUNCTIONS---------//

#pragma region "INITIALIZE FUNCTIONS" 
void initialize_flags()
{
	for (int x = 0; x < Global::ALU_FLAG_COUNT; x++)
	{
		ALU_Flags[x] = false;
	}
}

void initialize_forwarding_bus()
{
	for (int x = 0; x < Global::FORWARDING_BUSES; x++)
	{
		Forward_Bus[x].clear();
	}
}

void initialize_memory()
{
	for (int x = 0; x < Global::MEMORY_SIZE; x++)
	{
		Memory_Array[x] = 0;
	}
}

void initialize_Register_File()
{
	for (int x = 0; x < Global::ARCH_REGISTER_COUNT; x++)
	{
		Register_File[x].clear();
		Register_File[x].tag = Global::ARCH_REGISTERS(x);
	}
}

void initialize_most_recent()
{
	for (int x = 0; x < Global::ARCH_REGISTER_COUNT; x++)
	{
		Most_Recent_Reg[x] = Global::ARCH_REGISTERS::NA;
	}
}

void initialize_stalled_stages()
{
	for (int x = 0; x < Global::TOTAL_STAGES; x++)
	{
		Stalled_Stages[x] = false;
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
	initialize_Register_File();
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
	initialize_stalled_stages();
}
#pragma endregion

#pragma region displays
void displayRegisterFile()
{
	Global::Debug("");
	Global::Debug("-- Register File --");

	for (int x = 0; x < Global::ARCH_REGISTER_COUNT; x++)
	{
		Global::Debug("R" + Global::toString(Global::ARCH_REGISTERS(x)));
		Global::Debug("   status : " + Global::toString(Register_File[x].status));
		Global::Debug("   value  : " + to_string(Register_File[x].value));
	}
}

void displayMemory()
{
	Global::Debug("");
	Global::Debug("-- Memory Locations --");

	for each (int x in Memory_Array)
	{
		Global::Debug("Memory[" + to_string(x) + "] = " + to_string(Memory_Array[x]));
	}
}

void displayMostRecentlyUsed()
{

}

void displayStalledStages()
{
	Global::Debug("");
	Global::Debug("-- Stalled stages --");
	Global::Debug("Fetch		: " + to_string(Stalled_Stages[Global::STALLED_STAGE::FETCH]));
	Global::Debug("DECODE_RF	: " + to_string(Stalled_Stages[Global::STALLED_STAGE::DECODE_RF]));
	Global::Debug("ALU1		    : " + to_string(Stalled_Stages[Global::STALLED_STAGE::ALU1]));
	Global::Debug("ALU2		    : " + to_string(Stalled_Stages[Global::STALLED_STAGE::ALU2]));
	Global::Debug("BRANCH		: " + to_string(Stalled_Stages[Global::STALLED_STAGE::BRANCH]));
	Global::Debug("DELAY		: " + to_string(Stalled_Stages[Global::STALLED_STAGE::DELAY]));
	Global::Debug("MEMORY		: " + to_string(Stalled_Stages[Global::STALLED_STAGE::MEMORY]));
	Global::Debug("WRITEBACK	: " + to_string(Stalled_Stages[Global::STALLED_STAGE::WRITEBACK]));
	Global::Debug("");
}
#pragma endregion

string trim(string &str)
{
	size_t first = str.find_first_not_of(' ');
	size_t last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
}

void user_interface() //char * input_file, char * output_file)
{
	string	infile_name = "",
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


	if (!DEBUG)
	{
		while (infile_name == "")
		{
			cout << "please enter input instruction file: ";
			getline(cin, infile_name);
		}
	}
	else
	{
		infile_name = "test.txt";
	}

	fetch->setFile(infile_name);

	if (!DEBUG)
	{

		cout << "please enter output file (or blank): ";
		getline(cin, outfile_name);
	}

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

	Global::setOutFile(outfile_name);
	//}
}

//MAIN PROGRAM
int _tmain(int argc, char* argv[])
{
#pragma region LOCAL VARIABLES
	
	fetch = new Fetch();
	decode = new Decode();
	alu1 = new ALU1();
	alu2 = new ALU2();
	branch = new Branch();
	delay = new Delay();
	memory = new Memory();
	writeBack = new WriteBack();
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
	Global::apexStruct garbage_struct;
#pragma endregion //LOCAL VARIABLES

	user_interface();// input_file_name, output_file_name);
	
	if (input_file)
	{
		while (command != "end")
		{

			cout << endl
				<< endl
				<< endl
				<< "Available commands:" << endl
				<< "   initialize   : initialize pipeline" << endl
				<< "   simulate <n> : simulate n number of pipeline steps" << endl
				<< "   display      : display contents of the pipeline" << endl
				<< "   commands     : display command list" << endl
				<< "   end          : stop execution" << endl
				<< endl;
			getline(cin, command);
			if (command != "")
			{
				command = trim(command);
				std::transform(command.begin(), command.end(), command.begin(), ::tolower);
			}

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

			if (command == "display" )
			{
				cout << "Writing pipeline stages..." << endl;
				Global::Debug("Writing pipeline stages...");
				fetch->display();
				decode->display();
				alu1->display();
				alu2->display();
				branch->display();
				delay->display();
				memory->display();
				writeBack->display();
				displayRegisterFile();
				displayMemory();
				displayStalledStages();
				cout << "Writing pipeline stages Complete" << endl;
				Global::Debug("Writing pipeline stages Complete");
			}

#pragma endregion //BASIC COMMANDS

#pragma region SIMULATION
			string substr = command.substr(0, 8);

			if (substr.compare(0, 8, "simulate") == 0) //command is simulate
			{
				//get number of iterations to simulate
				tempS = command.substr(9, command.length() - 8);
				n = atoi(tempS.c_str());

				while (!HALT && n > 0 && pipelineHasData)
				{
					//start pipeline
					if (!Stalled_Stages[Global::STALLED_STAGE::DECODE_RF] && !fetch->endOfFile())
					{

						pipeline_struct_fetch = fetch->run(PC, Forward_Bus, Stalled_Stages);
						if (DEBUG)
							fetch->display();

						/*
						if (DEBUG)
						{
							Global::Debug(" ");
							Global::Debug("Register File Contents");
							for (int x = 0; x < Global::ARCH_REGISTER_COUNT; x++)
							{
								Global::Debug("Register : " + Global::toString(Global::ARCH_REGISTERS(x)));
								Global::Debug("  status : " + Global::toString(Register_File[Global::ARCH_REGISTERS(x)].status));
								Global::Debug("  value  : " + to_string(Register_File[Global::ARCH_REGISTERS(x)].value));
							}
						}
						*/
					}
					else if (fetch->endOfFile())
					{
						pipeline_struct_fetch = garbage_struct;
					}
					else
					{
						Stalled_Stages[Global::STALLED_STAGE::FETCH] = true;
						if (DEBUG)
							Global::Debug("FETCH STALLED!");
					}

					if (!Stalled_Stages[Global::STALLED_STAGE::ALU1]
						&& !Stalled_Stages[Global::STALLED_STAGE::BRANCH])
					{
						pipeline_struct_decode = decode->run(Register_File, Forward_Bus, Stalled_Stages, Most_Recent_Reg);
						if (DEBUG)
							decode->display();

						/*if (DEBUG)
						{
							Global::Debug(" ");
							Global::Debug("Most Recently Used Register");
							for (int x = 0; x < Global::ARCH_REGISTER_COUNT; x++)
							{
								Global::Debug("Register " + Global::toString(Global::ARCH_REGISTERS(x))
									+ " : " + to_string(Most_Recent_Reg[Global::ARCH_REGISTERS(x)]));
							}
						}*/
					}
					else
					{
						Stalled_Stages[Global::STALLED_STAGE::DECODE_RF] = true;
						if (DEBUG)
							Global::Debug("DECODE STALLED!");
					}

					if (!Stalled_Stages[Global::STALLED_STAGE::ALU2])
					{
						pipeline_struct_alu1 = alu1->run(Forward_Bus, Stalled_Stages);
						if (DEBUG)
							alu1->display();
					}
					else
					{
						Stalled_Stages[Global::STALLED_STAGE::ALU1] = true;
						if (DEBUG)
							Global::Debug("ALU1 STALLED!");
					}

					if (!Stalled_Stages[Global::STALLED_STAGE::MEMORY])
					{
						pipeline_struct_alu2 = alu2->run(ALU_Flags, Forward_Bus, Stalled_Stages);
						if (DEBUG)
							alu2->display();
					}
					else
					{
						Stalled_Stages[Global::STALLED_STAGE::ALU2] = true;
						if (DEBUG)
							Global::Debug("ALU2 STALLED!");
					}

					if (!Stalled_Stages[Global::STALLED_STAGE::DELAY])
					{
						pipeline_struct_branch = branch->run(PC, Forward_Bus, Stalled_Stages, Register_File[Global::ARCH_REGISTERS::X]);
						if (DEBUG)
							branch->display();
					}
					else
					{
						Stalled_Stages[Global::STALLED_STAGE::BRANCH] = true;
						if (DEBUG)
							Global::Debug("ALU2 STALLED!");
					}

					if (!Stalled_Stages[Global::STALLED_STAGE::MEMORY])
					{
						pipeline_struct_delay = delay->run(Stalled_Stages);
						if (DEBUG)
							delay->display();
					}
					else
					{
						Stalled_Stages[Global::STALLED_STAGE::DELAY] = true;
						if (DEBUG)
							Global::Debug("ALU2 STALLED!");
					}
					
					pipeline_struct_memory = memory->run(Forward_Bus, Stalled_Stages, Memory_Array);
					if (DEBUG)
						memory->display();

					writeBack->run(Register_File, Forward_Bus, Most_Recent_Reg, &HALT);
					if (DEBUG)
						writeBack->display();

					/*if (DEBUG)
					{
						Global::Debug(" ");
						Global::Debug("Register File Contents");
						for (int x = 0; x < Global::ARCH_REGISTER_COUNT; x++)
						{
							Global::Debug("Register : " + Global::toString(Global::ARCH_REGISTERS(x)));
							Global::Debug("  status : " + Global::toString(Register_File[Global::ARCH_REGISTERS(x)].status));
							Global::Debug("  value  : " + to_string(Register_File[Global::ARCH_REGISTERS(x)].value));
						}
					
						Global::Debug(" ");
						Global::Debug("Most Recently Used Register");
						for (int x = 0; x < Global::ARCH_REGISTER_COUNT; x++)
						{
							Global::Debug("Register " + Global::toString(Global::ARCH_REGISTERS(x))
								+ " : " + to_string(Most_Recent_Reg[Global::ARCH_REGISTERS(x)]));
						}
					}*/

					

					//set up for the next cycle
					if (!Stalled_Stages[Global::STALLED_STAGE::DECODE_RF] && !Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].updatePC)
					{
						decode->setPipelineStruct(pipeline_struct_fetch);
						pipeline_struct_fetch.clear();
						PC++;
					}
					else if (!Stalled_Stages[Global::STALLED_STAGE::DECODE_RF])
					{
						decode->setPipelineStruct(garbage_struct);
					}

					if (Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].updatePC)
					{
						decode->setPipelineStruct(garbage_struct);
						Global::Debug("....");
						Global::Debug("Flushing Fetch!");
						Global::Debug("Flushing Decode/RF");
						Global::Debug("....");
					}

					switch (pipeline_struct_decode.instruction.op_code)
					{
						case Global::OPCODE::ADD:
						case Global::OPCODE::ADDL:
						case Global::OPCODE::AND:
						case Global::OPCODE::ANDL:
						case Global::OPCODE::EX_OR:
						case Global::OPCODE::EX_ORL:
						case Global::OPCODE::MOVC:
						case Global::OPCODE::MUL:
						case Global::OPCODE::MULL:
						case Global::OPCODE::OR:
						case Global::OPCODE::ORL:
						case Global::OPCODE::SUB:
						case Global::OPCODE::SUBL:
						case Global::OPCODE::HALT:
						case Global::OPCODE::LOAD:
						case Global::OPCODE::STORE:
							if (!Stalled_Stages[Global::STALLED_STAGE::DECODE_RF] && !Stalled_Stages[Global::STALLED_STAGE::ALU1] &&  !Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].updatePC)
							{
								alu1->setPipelineStruct(pipeline_struct_decode);
							}
							else if (!Stalled_Stages[Global::STALLED_STAGE::ALU1] || Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].updatePC)
							{
								alu1->setPipelineStruct(garbage_struct);
							}

							if (!Stalled_Stages[Global::STALLED_STAGE::BRANCH] )
							{
								branch->setPipelineStruct(garbage_struct);
							}
							break;
						case Global::OPCODE::BAL:
						case Global::OPCODE::BNZ:
						case Global::OPCODE::BZ:
						case Global::OPCODE::JUMP:
							if (!Stalled_Stages[Global::STALLED_STAGE::DECODE_RF] && !Stalled_Stages[Global::STALLED_STAGE::BRANCH] && !Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].updatePC)
							{
								branch->setPipelineStruct(pipeline_struct_decode);	
							}
							else if (!Stalled_Stages[Global::STALLED_STAGE::BRANCH])
							{
								branch->setPipelineStruct(garbage_struct);
							}

							if (!Stalled_Stages[Global::STALLED_STAGE::ALU1])
							{
								alu1->setPipelineStruct(garbage_struct);
							}
							break;
						default:
							break;
					}
					
					if (!Stalled_Stages[Global::STALLED_STAGE::ALU1] && !Stalled_Stages[Global::STALLED_STAGE::ALU2])
					{
						alu2->setPipelineStruct(pipeline_struct_alu1);
						pipeline_struct_alu1.clear();
					}
					else if (!Stalled_Stages[Global::STALLED_STAGE::ALU2])
					{
						alu2->setPipelineStruct(garbage_struct);
					}

					if (!Stalled_Stages[Global::STALLED_STAGE::BRANCH] && !Stalled_Stages[Global::STALLED_STAGE::DELAY])
					{
						delay->setPipelineStruct(pipeline_struct_branch);
						pipeline_struct_branch.clear();
					}
					else if (!Stalled_Stages[Global::STALLED_STAGE::DELAY])
					{
						delay->setPipelineStruct(garbage_struct);
					}

					if (!Stalled_Stages[Global::STALLED_STAGE::ALU2] 
						&& !Stalled_Stages[Global::STALLED_STAGE::DELAY] 
						&& !Stalled_Stages[Global::STALLED_STAGE::MEMORY])
					{
						if (pipeline_struct_alu2.pc_value < pipeline_struct_delay.pc_value)
						{
							memory->setPipelineStruct(pipeline_struct_alu2);
							pipeline_struct_alu2.clear();
						}
						else
						{
							memory->setPipelineStruct(pipeline_struct_delay);
							pipeline_struct_delay.clear();
						}
					}
					else if (!Stalled_Stages[Global::STALLED_STAGE::MEMORY])
					{
						memory->setPipelineStruct(garbage_struct);
					}

					writeBack->setPipelineStruct(pipeline_struct_memory);
					if (!Stalled_Stages[Global::STALLED_STAGE::MEMORY])
					{
						pipeline_struct_memory.clear();
					}

					if (decode->hasValidData() 
						|| alu1->hasValidData() 
						|| alu2->hasValidData()
						|| branch->hasValidData()
						|| delay->hasValidData()
						|| memory->hasValidData()
						|| writeBack->hasValidData())
					{
						pipelineHasData = true;
					}
					else
					{
						pipelineHasData = false;
					}

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

	/*
	//destruct
	fetch->~Fetch();
	decode->~Decode();
	alu1->~ALU1();
	alu2->~ALU2();
	branch->~Branch();
	delay->~Delay();
	memory->~Memory();
	writeBack->~WriteBack();
	*/

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