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
#include "Multiply.h"
#include "Branch.h"
#include "Delay.h"
#include "Memory.h"
#include "WriteBack.h"

#define DEBUG_INPUT 1
#define DEBUG_OUTPUT 1

#define DEFAULT_URF_SIZE 32

using namespace std;

//--------VARIABLES--------//

//program counter
int PC = 4000;

//stalled stages
bool Stalled_Stages[Global::STALLED_STAGE::FINAL_STALLED_STAGE_TOTAL];

//flags from ALU
bool ALU_Flags[Global::FLAGS::FINAL_FLAGS_TOTAL];

//Register file
int urf_size = DEFAULT_URF_SIZE;
Global::Register_Info * Register_File;

//forwarding bus
//Global::Forwarding_Info Forward_Bus[Global::FINAL_FORWARD_TYPE_TOTAL];
Global::Forwarding_Info Forward_Bus[Global::FORWARD_TYPE::FINAL_FORWARD_TYPE_TOTAL];

//Memory - 4 bytes wide, 0 to 3999
int Memory_Array[Global::MEMORY_SIZE];

//contains PC values
//int Most_Recent_Reg[Global::FINAL_ARCH_REGISTERS_ITEM];

//Reorder Buffer (ROB)
Global::Reorder_Buffer ROB;

//Front End Rename Table (RAT)
Global::Rename_Table Front_End_RAT;

//Back End Rename Table (RAT)
Global::Rename_Table Back_End_RAT;

//identify when instructions are still in the pipeline
bool pipelineHasData = true;

Fetch * fetch;
Decode * decode;
ALU1 * alu1;
ALU2 * alu2;
Multiply * multiply;
Branch * branch;
Delay * delay;
Memory * memory;
WriteBack * writeBack;

Global::apexStruct pipeline_struct_fetch;
Global::apexStruct pipeline_struct_decode;
Global::apexStruct pipeline_struct_alu1;
Global::apexStruct pipeline_struct_alu2;
Global::apexStruct pipeline_struct_multiply;
Global::apexStruct pipeline_struct_branch;
Global::apexStruct pipeline_struct_delay;
Global::apexStruct pipeline_struct_memory;
Global::apexStruct garbage_struct;

bool HALT = false;

int iteration = 1;

//-----------FUNCTIONS---------//

#pragma region "INITIALIZE FUNCTIONS" 
void initialize_flags()
{
	for (int x = 0; x < Global::FLAGS::FINAL_FLAGS_TOTAL; x++)
	{
		ALU_Flags[x] = false;
	}
}

void initialize_forwarding_bus()
{
	for (int x = 0; x < Global::FORWARD_TYPE::FINAL_FORWARD_TYPE_TOTAL; x++)
	{
		Forward_Bus[x].flag = Global::FLAGS::CLEAR;
		Forward_Bus[x].opcode = Global::OPCODE::NONE;
		Forward_Bus[x].pc_value = -1;
		Forward_Bus[x].updatePC = false;
		Forward_Bus[x].target = INT_MAX;
		Forward_Bus[x].tag = Global::REGISTERS::UNA;
		Forward_Bus[x].reg_info.status = Global::REGISTER_ALLOCATION::REG_UNALLOCATED;
		//Forward_Bus[x].reg_info.tag = Global::ARCH_REGISTERS::NA;
		Forward_Bus[x].reg_info.value = INT_MAX;
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
	for (int x = 0; x < urf_size; x++)
	{
		Register_File[x].clear();
	}
}

/*
void initialize_most_recent()
{
	for (int x = 0; x < Global::FINAL_ARCH_REGISTERS_ITEM; x++)
	{
		Most_Recent_Reg[x] = Global::ARCH_REGISTERS::NA;
	}
}
*/

void initialize_stalled_stages()
{
	for (int x = 0; x < Global::STALLED_STAGE::FINAL_STALLED_STAGE_TOTAL; x++)
	{
		Stalled_Stages[x] = false;
	}
}

void initialize_rob()
{
	ROB.head = 0;
	ROB.tail = 0;
	for (int x = 0; x < Global::ROB_SIZE; x++)
	{
		ROB.entries[x].clear();
	}
}

void initialize_rename_table(){
	Front_End_RAT.clear();
	Back_End_RAT.clear();
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
	cout << "Initializing Reorder Buffer" << endl
		<< "..." << endl;
	initialize_rob();
	cout << "Initializing Reorder Buffer" << endl;
	cout << "Initializing Rename Table" << endl
		<< "..." << endl;
	initialize_rename_table();
	cout << "Initializing Rename Table" << endl;
	/*
	cout << "Resetting History" << endl
		<< "..." << endl;
	initialize_most_recent();
	cout << "Resetting History Complete" << endl;
	*/
	cout << "Resetting PC" << endl
		<< "..." << endl;
	PC = 4000;
	cout << "Resetting PC Complete" << endl;
	cout << "Resetting Stalled Stages" << endl
		<< "..." << endl;
	initialize_stalled_stages();
	cout << "Resetting Stalled Stages Complete" << endl;
	HALT = false;
	iteration = 1;

	cout << "Resetting Pipeline Stages" << endl
		<< "..." << endl;
	pipeline_struct_fetch = garbage_struct;
	pipeline_struct_decode = garbage_struct;
	pipeline_struct_alu1 = garbage_struct;
	pipeline_struct_alu2 = garbage_struct;
	pipeline_struct_branch = garbage_struct;
	pipeline_struct_delay = garbage_struct;
	pipeline_struct_memory = garbage_struct;
	decode->setPipelineStruct(garbage_struct);
	alu1->setPipelineStruct(garbage_struct);
	alu2->setPipelineStruct(garbage_struct);
	branch->setPipelineStruct(garbage_struct);
	delay->setPipelineStruct(garbage_struct);
	memory->setPipelineStruct(garbage_struct);
	writeBack->setPipelineStruct(garbage_struct);
	cout << "Resetting Pipeline Stages Complete" << endl;
}
#pragma endregion

#pragma region displays
void displayRegisterFile()
{
	Global::Debug("");
	Global::Debug("-- Register File --");

	//for (int x = 0; x < Global::FINAL_ARCH_REGISTERS_ITEM; x++)
	for (int x = 0; x < Global::REGISTERS::FINAL_REGISTERS_TOTAL; x++)
	{
		//Global::Debug(Global::toString(Global::ARCH_REGISTERS(x));
		Global::Debug(Global::toString(Global::REGISTERS(x)));
		Global::Debug("   status : " + Global::toString(Register_File[x].status));
		Global::Debug("   value  : " + to_string(Register_File[x].value));
	}
}

void displayMemory()
{
	Global::Debug("");
	Global::Debug("-- Memory Locations --");

	for (int x = 0; x < 100; x++)
	{
		Global::Debug("Memory[" + to_string(x) + "] = " + to_string(Memory_Array[x]));
	}
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

void displayROB()
{
	Global::Debug("-- ROB Information --");
	Global::Debug("  HEAD = " + to_string(ROB.head));
	Global::Debug("  TAIL = " + to_string(ROB.tail));
	for (int x = ROB.head; x <= ROB.tail; x++)
	{

	}
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


	if (!DEBUG_INPUT)
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

	cout << endl;

	if (!DEBUG_INPUT)
	{

		cout << "please enter output file (or blank): ";
		getline(cin, outfile_name);
	}

	if (outfile_name == "")
	{
		cout << endl << "using default output file: apex_output[date_time].txt" << endl;
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
	multiply = new Multiply();
	branch = new Branch();
	delay = new Delay();
	memory = new Memory();
	writeBack = new WriteBack();
	ifstream input_file;
	ofstream output_file;
	char * input_file_name = NULL;
	char * output_file_name = NULL;
	string command = "",
		tempS = "",
		substr = "";

	int n = 0;

	Register_File = new Global::Register_Info[DEFAULT_URF_SIZE];

	bool sim_started = false;

#pragma endregion //LOCAL VARIABLES
	
	initialize_pipeline();

	user_interface();// input_file_name, output_file_name);
	
	if (input_file)
	{
		while (command != "end")
		{
			substr = "";

			cout << endl
				<< endl
				<< endl
				<< "Available commands:" << endl
				<< "   initialize             : initialize pipeline" << endl
				<< "   simulate <n>           : simulate n number of pipeline steps" << endl 
				<< "   Set_URF_size <n>       : before simulation, set the number of registers" << endl
				<< "   Print_map_tables       : prints front end rename table and "<<endl
				<< "                            back-end alias table" << endl
				<< "   Print_IQ               : prints issue quque entries and their status" << endl
				<< "   Print_ROB              : prints current ROB contents" << endl
				<< "   Print_URF              : prints contents of URF and status" << endl
				<< "   Print_Memory <a1> <a2> : prints out memory ranging from a1 to a2" << endl
				<< "   Print_Stats            : prints the IPC realized up to this point, " << endl
				<< "                            number of cycles which dispatched was stalled, " << endl
				<< "                            number of cycles for which no issues have taken place," << endl
				<< "                            number of LOAD and STORE instructions committed" << endl
				<< "   display                : display contents of the pipeline" << endl
				<< "   commands               : display command list" << endl
				<< "   end                    : stop execution" << endl
				<< endl
				<< ">> ";
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
				system("cls");
			}

			if (command == "display" )
			{
				cout << "Writing pipeline stages..." << endl;
				/*
				Global::Debug("Writing pipeline stages...");
				if (!Stalled_Stages[Global::STALLED_STAGE::FETCH])
					fetch->display();
				else
					Global::Debug("-- Fetch Stalled -- ");

				if (!Stalled_Stages[Global::STALLED_STAGE::DECODE_RF])
					decode->display();
				else
					Global::Debug("-- Decode Stalled -- " + decode->getInstruction());

				if (!Stalled_Stages[Global::STALLED_STAGE::ALU1])
					alu1->display();
				else
					Global::Debug("-- ALU1 Stalled -- " + alu1->getInstruction());

				if (!Stalled_Stages[Global::STALLED_STAGE::ALU2])
					alu2->display();
				else
					Global::Debug("-- ALU2 Stalled -- " + alu2->getInstruction());

				if (!Stalled_Stages[Global::STALLED_STAGE::BRANCH])
					branch->display();
				else
					Global::Debug("-- Branch Stalled -- " + branch->getInstruction());

				if (!Stalled_Stages[Global::STALLED_STAGE::DELAY])
					delay->display();
				else
					Global::Debug("-- Delay Stalled -- " + delay->getInstruction());

				if (!Stalled_Stages[Global::STALLED_STAGE::MEMORY])
					memory->display();
				else
					Global::Debug("-- Memory Stalled -- " + memory->getInstruction());
				writeBack->display();
				*/
				cout << "Writing pipeline stages Complete" << endl
					<< endl;
				//Global::Debug("Writing pipeline stages Complete");
				cout << "Writing Register File Contents..." << endl;
				displayRegisterFile();
				cout << "Writing Register File Contents Complete" << endl
					<< endl
					<< "Writing Memory Contents..." << endl;
				displayMemory();
				cout << "Writing Memory Contents Complete" << endl;
				string cmd = "notepad.exe " + Global::getOutFile();
				system(cmd.c_str());
			}

			if (command == "print_map_tables")
			{

			}

			if (command == "print_iq")
			{

			}

			if (command == "print_rob")
			{

			}

			if (command == "print_urf")
			{

			}

			if (command == "print_stats")
			{

			}

			substr = command.substr(0, 8);

			if ((substr.compare(0, 8, "set_urf_") == 0) //command is set urf size
				&& (!sim_started))
			{
				tempS = command.substr(13, command.length() - 12);
				urf_size = atoi(tempS.c_str());
				Register_File = new Global::Register_Info[urf_size];
			}
			else
			{
				cout << "** ERROR - cannot set URF size once simulation has started! **" << endl;
			}

#pragma endregion //BASIC COMMANDS

/*SIMULATION*/

			if (substr.compare(0, 8, "simulate") == 0) //command is simulate
			{
				//get number of iterations to simulate
				tempS = command.substr(9, command.length() - 8);
				n = atoi(tempS.c_str());

				//keep going until we reach a HALT command, 
				//we complete the # of iterations specified
				//and while the pipeline still has data in it
				while (!HALT && n > 0 && pipelineHasData)
				{
					Global::Debug("\nIteration " + to_string(iteration));

#pragma region EXECUTE_PIPELINE

					/*********************
					 *  EXECUTE STAGES   *
					 *********************/

					//update ROB
					if (ROB.head != ROB.tail)
					{
						switch (ROB.entries[ROB.head].type)
						{
							case Global::INSTRUCTION_TYPE::BRANCH_TYPE:
								break;
							case Global::INSTRUCTION_TYPE::LOAD_TYPE:
								break;
							case Global::INSTRUCTION_TYPE::REG_TO_REG_TYPE:
								//update back end RAT
								Back_End_RAT.reg[ROB.entries[ROB.head].destArchReg] = ROB.entries[ROB.head].destReg;
								Back_End_RAT.rob_loc[ROB.entries[ROB.head].destArchReg] = -1;
								Back_End_RAT.src_bit[ROB.entries[ROB.head].destArchReg] = Global::SOURCES::REGISTER_FILE;

								//Mark register file as committed
								Register_File[ROB.entries[ROB.head].destReg].status = Global::REGISTER_ALLOCATION::ALLOC_COMMIT;
								Register_File[ROB.entries[ROB.head].destReg].value = ROB.entries[ROB.head].result;
								break;
							case Global::INSTRUCTION_TYPE::STORE_TYPE:
								break;
							default:
								break;
						}

						//update ROB head pointer
						ROB.head = (ROB.head + 1) % Global::ROB_SIZE;
					}

					//run writeback
					//HALT = writeBack->run(Register_File, Forward_Bus, Back_End_RAT);
					HALT = writeBack->run(Forward_Bus, ROB, Register_File, Back_End_RAT);

					if (!HALT)
					{
						/*
						//run memory, since wb cannot stall
						pipeline_struct_memory = memory->run(Forward_Bus, Stalled_Stages, Memory_Array);

						//as long as memeory is not stalled, run Delay
						if (!Stalled_Stages[Global::STALLED_STAGE::MEMORY])
						{
							pipeline_struct_delay = delay->run(Stalled_Stages);
						}
						//or stall delay
						else
						{
							Stalled_Stages[Global::STALLED_STAGE::DELAY] = true;
						}

						//as long as DELAY is not stalled, run branch
						if (!Stalled_Stages[Global::STALLED_STAGE::DELAY])
						{
							pipeline_struct_branch = branch->run(PC, Forward_Bus, Stalled_Stages, Register_File[Global::REGISTERS::UX]);
						}
						//or stall branch
						else
						{
							Stalled_Stages[Global::STALLED_STAGE::BRANCH] = true;
						}

						//flush decode if we have to update the PC
						//fetch already takes care of this internally, no need to 
						//call it separately
						if (Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].updatePC)
						{
							decode->setPipelineStruct(garbage_struct);
							Global::Debug("....");
							Global::Debug("Flushing Fetch!");
							Global::Debug("Flushing Decode/RF");
							if (!Stalled_Stages[Global::STALLED_STAGE::ALU1])
							{
								alu1->setPipelineStruct(garbage_struct);
								Global::Debug("Flushing ALU1");
							}
							Global::Debug("...."); 
						}
						*/

						//Run Multiply
						pipeline_struct_multiply = multiply->run(Forward_Bus, Register_File, ROB);

						//Run AL2
						pipeline_struct_alu2 = alu2->run(Forward_Bus, Stalled_Stages, Register_File, ROB);

						//Run ALU1
						pipeline_struct_alu1 = alu1->run(Forward_Bus, Stalled_Stages);

						//Decode handles delays based on OPCODE internally and update Stalled_Stages
						pipeline_struct_decode = decode->run(Register_File, urf_size, Forward_Bus, Stalled_Stages, ROB, Front_End_RAT);
						
						//as long as decode is not stalled and we still have data to read from the
						//input file, fetch the next command
						if ((!Stalled_Stages[Global::STALLED_STAGE::DECODE_RF]
							&& !fetch->endOfFile()
							&& (pipeline_struct_decode.instruction.op_code != Global::OPCODE::HALT))
							|| (Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].updatePC))
						{
							pipeline_struct_fetch = fetch->run(PC, Forward_Bus, Stalled_Stages);
						}
						//if eof, the feed garbage to the next stage
						else if (fetch->endOfFile())
						{
							pipeline_struct_fetch = garbage_struct;
							Global::Debug("Input File Complete");
						}
						//or we stall
						else
						{
							Stalled_Stages[Global::STALLED_STAGE::FETCH] = true;
						}
#pragma endregion //EXECUTE_PIPELINE

#pragma region DEBUG_OUTPUT
						if (DEBUG_OUTPUT)
						{
							if (!Stalled_Stages[Global::STALLED_STAGE::FETCH])
								fetch->display();
							else
								Global::Debug("-- Fetch Stalled -- ");

							if (!Stalled_Stages[Global::STALLED_STAGE::DECODE_RF])
								decode->display();
							else
								Global::Debug("-- Decode Stalled -- " + decode->getInstruction());

							if (!Stalled_Stages[Global::STALLED_STAGE::ALU1])
								alu1->display();
							else
								Global::Debug("-- ALU1 Stalled -- " + alu1->getInstruction());

							if (!Stalled_Stages[Global::STALLED_STAGE::ALU2])
								alu2->display();
							else
								Global::Debug("-- ALU2 Stalled -- " + alu2->getInstruction());

							if (!Stalled_Stages[Global::STALLED_STAGE::BRANCH])
								branch->display();
							else
								Global::Debug("-- Branch Stalled -- " + branch->getInstruction());

							if (!Stalled_Stages[Global::STALLED_STAGE::DELAY])
								delay->display();
							else
								Global::Debug("-- Delay Stalled -- " + delay->getInstruction());

							if (!Stalled_Stages[Global::STALLED_STAGE::MEMORY])
								memory->display();
							else
								Global::Debug("-- Memory Stalled -- " + memory->getInstruction());
							writeBack->display();
						}
#pragma endregion //DEBUG_OUTPUT

						//*************************
						//set up for the next cycle
						//*************************

						//as long as memory is not stalled and valid data to pass, move it on
						//if (memory->hasValidData() && !Stalled_Stages[Global::STALLED_STAGE::MEMORY])
						if (!Stalled_Stages[Global::STALLED_STAGE::MEMORY])
						{
							writeBack->setPipelineStruct(pipeline_struct_memory);
							pipeline_struct_memory = garbage_struct;
						}
						else
						{
							writeBack->setPipelineStruct(garbage_struct);
						}

						//send memory data based on pc value and make sure we ar enot stalled
						if (!Stalled_Stages[Global::STALLED_STAGE::MEMORY])
						{
							if ((pipeline_struct_alu2.pc_value < pipeline_struct_delay.pc_value)
								//&& alu2->hasValidData()
								&& !Stalled_Stages[Global::STALLED_STAGE::ALU2])
							{
								memory->setPipelineStruct(pipeline_struct_alu2);
								pipeline_struct_alu2 = garbage_struct;
							}
							else if ((pipeline_struct_delay.pc_value < pipeline_struct_alu2.pc_value)
								//&& delay->hasValidData()
								&& !Stalled_Stages[Global::STALLED_STAGE::DELAY])
							{
								memory->setPipelineStruct(pipeline_struct_delay);
								pipeline_struct_delay = garbage_struct;
							}
							else
							{
								memory->setPipelineStruct(garbage_struct);
							}
						}

						//if not stalled, pass branch output to delay
						if (!Stalled_Stages[Global::STALLED_STAGE::BRANCH]
							&& !Stalled_Stages[Global::STALLED_STAGE::DELAY])
							//&& branch->hasValidData())
						{
							delay->setPipelineStruct(pipeline_struct_branch);
							pipeline_struct_branch = garbage_struct;
						}
						//or as long as delay is not stalled, bubble
						else if (!Stalled_Stages[Global::STALLED_STAGE::DELAY])
						{
							delay->setPipelineStruct(garbage_struct);
						}

						//as long as we are not stalled, pass data
						if (!Stalled_Stages[Global::STALLED_STAGE::ALU1]
							&& !Stalled_Stages[Global::STALLED_STAGE::ALU2])
							//&& alu1->hasValidData())
						{
							alu2->setPipelineStruct(pipeline_struct_alu1);
							pipeline_struct_alu1 = garbage_struct;
						}
						//if alu2 is not stalled, just pass in bubble
						else if (!Stalled_Stages[Global::STALLED_STAGE::ALU2])
						{
							alu2->setPipelineStruct(garbage_struct);
						}

						//based on opcode....
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
							//if for the ALU, and we are not stalled, or branching, 
							//set up ALU1 with the output of code
							if (!Stalled_Stages[Global::STALLED_STAGE::DECODE_RF]
								&& !Stalled_Stages[Global::STALLED_STAGE::ALU1]
								&& !Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].updatePC)
								//&& decode->hasValidData())
							{
								alu1->setPipelineStruct(pipeline_struct_decode);
								pipeline_struct_decode = garbage_struct;
							}
							//feed in bubble if we are not stalled, the branch was taken
							//or if code does not have valid data (bubble)
							else if (!Stalled_Stages[Global::STALLED_STAGE::ALU1]
								|| Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].updatePC)
								//|| !decode->hasValidData())
							{
								alu1->setPipelineStruct(garbage_struct);
							}

							//set branch to bubble
							if (!Stalled_Stages[Global::STALLED_STAGE::BRANCH])
							{
								branch->setPipelineStruct(garbage_struct);
							}
							break;
						case Global::OPCODE::BAL:
						case Global::OPCODE::BNZ:
						case Global::OPCODE::BZ:
						case Global::OPCODE::JUMP:
							//if for the branch, and we are not stalled, or branching, 
							//set up branch with the output of code
							if (!Stalled_Stages[Global::STALLED_STAGE::DECODE_RF]
								&& !Stalled_Stages[Global::STALLED_STAGE::BRANCH]
								&& !Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].updatePC)
								//&& decode->hasValidData())
							{
								branch->setPipelineStruct(pipeline_struct_decode);
								pipeline_struct_decode = garbage_struct;
							}
							//feed in bubble if we are not stalled
							else if (!Stalled_Stages[Global::STALLED_STAGE::BRANCH])
							{
								branch->setPipelineStruct(garbage_struct);
							}
							//set branch to bubble
							if (!Stalled_Stages[Global::STALLED_STAGE::ALU1])
							{
								alu1->setPipelineStruct(garbage_struct);
							}
							break;
						default:
							break;
						}


						//as long as decode is not stalled, and we are not updating
						// the PC from a branch, and we still have data to read
						//from fetch, move the fetch output to the input of decode
						if (!Stalled_Stages[Global::STALLED_STAGE::DECODE_RF]
							//&& !fetch->endOfFile()
							&& !HALT)
						{
							decode->setPipelineStruct(pipeline_struct_fetch);
							pipeline_struct_fetch = garbage_struct;
							PC++;
						}
						//or if we are not stalled, then feed in garbage (bubble)
						else if (!Stalled_Stages[Global::STALLED_STAGE::DECODE_RF] || HALT)
						{
							decode->setPipelineStruct(garbage_struct);
						}


						//flush decode if we have to update the PC
						//fetch already takes care of this internally, no need to 
						//call it separately
						if (Forward_Bus[Global::FORWARD_TYPE::FROM_BRANCH].updatePC)
						{
							alu1->setPipelineStruct(garbage_struct);
							branch->setPipelineStruct(garbage_struct);
						}

						//see if we have any valid data to be
						//able to flush pipeline to completion
						if (decode->hasValidData()
							|| alu1->hasValidData()
							|| alu2->hasValidData()
							|| multiply->hasValidData()
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
					}
					else
					{
						cout << endl
							<< "Program encounted a HALT" << endl
							<< endl
							<< "Please re-initialize the program" << endl
							<< endl;
					}

					n--;
					iteration++;
				} //!HALT && n > 0
				cout << "." << endl
					<< "." << endl
					<< "." << endl
					<<"Simulation Complete" << endl;
			} // command == simulate n
		} //command != "end"
	}

/* END SIMULATION*/

	else
	{
		cout << "Error with input file: " << input_file_name << endl
			<< "EXITING" << endl;
	}

	input_file.close();
	Global::closeFile();

	//delete & free up memory
	delete fetch;
	delete decode;
	delete alu1;
	delete alu2;
	delete multiply;
	delete branch;
	delete delay;
	delete memory;
	delete writeBack;

	return 0;
}