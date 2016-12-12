/* Eric Garbade & Frank Zurla
 * CS520
 * Fall 2016
 * Professor Kanad Ghose
 * APEX (A Pipeline EXample) emulator Project 2
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
#include <sstream>

#include "Global.h"
#include "Fetch.h"
#include "Decode.h"
#include "Dispatch.h"
#include "IssueQueue.h"
#include "ALU1.h"
#include "ALU2.h"
#include "Multiply.h"
#include "Branch.h"
#include "LS1.h"
#include "LS2.h"
#include "WriteBack.h"

#define DEBUG_INPUT 1
#define DEBUG_OUTPUT 1

#define DEFAULT_URF_SIZE 32

using namespace std;

#pragma region VARIABLES

//program counter
int PC = 4000;
int instructions_through_wb = 0;

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
Global::Front_End_Rename_Entry Front_End_RAT[Global::ARCH_REGISTERS::FINAL_ARCH_REGISTERS_ITEM];

//Back End Rename Table (RAT)
Global::Back_End_Rename_Entry Back_End_RAT[Global::ARCH_REGISTERS::FINAL_ARCH_REGISTERS_ITEM];

//Program Statistics
Global::Statistics stats;

//identify when instructions are still in the pipeline
bool pipelineHasData = true;

Fetch * fetch;
Decode * decode;
Dispatch * dispatch;
IssueQueue * iq;
ALU1 * alu1;
ALU2 * alu2;
Multiply * multiply;
Branch * branch;
LS1 * ls1;
LS2 * ls2;
WriteBack * writeBack;

Global::apexStruct pipeline_struct_fetch;
Global::apexStruct pipeline_struct_decode;
Global::apexStruct pipeline_struct_dispatch;
vector<Global::apexStruct> pipeline_structs_iq;
Global::apexStruct pipeline_struct_alu1;
Global::apexStruct pipeline_struct_alu2;
Global::apexStruct pipeline_struct_multiply;
Global::apexStruct pipeline_struct_ls1;
Global::apexStruct pipeline_struct_ls2;
Global::apexStruct pipeline_struct_branch;
Global::apexStruct garbage_struct;

bool HALT = false;

int iteration = 1;

#pragma endregion VARIABLES

#pragma region "INITIALIZE FUNCTIONS" 
void initialize_forwarding_bus();
void initialize_memory();
void initialize_Register_File();
void initialize_stalled_stages();
void initialize_rob();
void initialize_rename_table();
void initialize_stats();
void initialize_pipeline();
#pragma endregion INITIALIZE FUNCTIONS

#pragma region DISPLAY FUNCTIONS
void displayRegisterFile();
void displayFRAT();
void displayRRAT();
void displayMemory(int a1, int a2);
void displayStalledStages();
void displayROB();
void displayStats();
#pragma endregion DISPLAY FUNCTIONS

string trim(string &str);

void user_interface();

//MAIN PROGRAM
int _tmain(int argc, char* argv[])
{
#pragma region LOCAL VARIABLES
	
	fetch = new Fetch();
	decode = new Decode();
	dispatch = new Dispatch();
	iq = new IssueQueue();
	alu1 = new ALU1();
	alu2 = new ALU2();
	multiply = new Multiply();
	branch = new Branch();
	ls1 = new LS1();
	ls2 = new LS2();
	writeBack = new WriteBack();
	char * input_file_name = NULL;
	char * output_file_name = NULL;
	string command = "",
		tempS = "",
		substr = "";
	vector<Global::apexStruct> issued_instructions;

	int n = 0;

	Register_File = new Global::Register_Info[DEFAULT_URF_SIZE];

	bool sim_started = false;

#pragma endregion //LOCAL VARIABLES
	
	initialize_pipeline();

	user_interface();
	
	if (fetch->fileIsGood())
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
				<< "   Print_Stats            : 1) prints the IPC realized up to this point, " << endl
				<< "                            2) number of cycles which dispatched was stalled, " << endl
				<< "                            3) number of cycles for which no issues have " << endl
				<<"                                taken place," << endl
				<< "                            4) number of LOAD and STORE instructions committed" << endl
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
				cout << "Writing pipeline stages..." << endl
					<< "..." << endl
					<< "Writing pipeline stages Complete" << endl
					<< endl;
				cout << "Writing Register File Contents" << endl
					<< "..." << endl;
				displayRegisterFile();
				cout << "Writing Register File Contents Complete" << endl
					<< endl
					<< "Writing ROB Contents..." << endl
					<< "..." << endl;
				displayROB();
				cout << "Writing ROB Contents Complete" << endl
					<< endl
					<< "Writing Front-End RAT Contents..." << endl
					<< "..." << endl;
				displayFRAT();
				cout << "Writing Front-End RAT Contents Complete" << endl
					<< endl
					<< "Writing Back-End RAT Contents..." << endl
					<< "..." << endl;
				displayRRAT(); 
				cout << "Writing Back-End RAT Contents Complete" << endl;
				string cmd = "notepad.exe " + Global::getOutFile();
				system(cmd.c_str());
			}

			if (command == "print_map_tables")
			{
				cout << "Writing Front-End RAT Contents..." << endl
					<< "..." << endl;
				displayFRAT();
				cout << "Writing Front-End RAT Contents Complete" << endl
					<< endl
					<< "Writing Back-End RAT Contents..." << endl
					<< "..." << endl;
				displayRRAT();
				cout << "Writing Back-End RAT Contents Complete" << endl;
			}

			if (command == "print_iq")
			{

			}

			if (command == "print_rob")
			{
				displayROB();
			}

			if (command == "print_urf")
			{
				displayRegisterFile();
				string cmd = "notepad.exe " + Global::getOutFile();
				system(cmd.c_str());
			}

			if (command == "print_stats")
			{
				displayStats();
				string cmd = "notepad.exe " + Global::getOutFile();
				system(cmd.c_str());
			}

			substr = command.substr(0, 8);

			if ((substr.compare(0, 8, "set_urf_") == 0) //command is set urf size
				&& (!sim_started))
			{
				tempS = command.substr(13, command.length() - 12);
				urf_size = atoi(tempS.c_str());
				Register_File = new Global::Register_Info[urf_size+1];
			}
			else
			{
				cout << "** WANRNING - cannot set URF size once simulation has started! **" << endl;
			}

			if ((substr.compare(0, 11, "print_memory") == 0))
			{
				std::istringstream iss(command);
				string instruction;
				string a1S;
				string a2S;
				iss >> instruction;
				iss >> a1S;
				iss >> a2S;
				int a1 = stoi(a1S);
				int a2 = stoi(a2S);
				displayMemory(a1, a2);
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
					Global::Output("\n------------------------------------");
					Global::Output("Iteration " + to_string(iteration));

#pragma region EXECUTE_PIPELINE

					/*********************
					 *  EXECUTE STAGES   *
					 *********************/

					//update ROB
					if (ROB.head != ROB.tail)
					{
						if ((ROB.entries[ROB.head].type == Global::INSTRUCTION_TYPE::LOAD_TYPE)
							|| (ROB.entries[ROB.head].type == Global::INSTRUCTION_TYPE::STORE_TYPE))
						{
							ROB.entries[ROB.head].alloc = Global::ROB_ALLOCATION::COMPLETE;
						}

						if (ROB.entries[ROB.head].alloc == Global::ROB_ALLOCATION::COMPLETE)
						{
							int old_reg = 0;
							switch (ROB.entries[ROB.head].type)
							{
							case Global::INSTRUCTION_TYPE::BRANCH_TYPE:
								if (ROB.entries[ROB.head].destArchReg == Global::ARCH_REGISTERS::X)
								{
									//free up reg
									old_reg = Back_End_RAT[Global::ARCH_REGISTERS::X].reg;
									Register_File[old_reg].status = Global::REGISTER_ALLOCATION::REG_UNALLOCATED;
									Register_File[ROB.entries[ROB.head].destReg].value = ROB.entries[ROB.head].result;

									//update back end RAT
									Back_End_RAT[Global::ARCH_REGISTERS::X].reg = ROB.entries[ROB.head].destReg;
								}
								break;
							
							case Global::INSTRUCTION_TYPE::REG_TO_REG_TYPE:
								//free up reg
								old_reg = Back_End_RAT[ROB.entries[ROB.head].destArchReg].reg;
								Register_File[old_reg].status = Global::REGISTER_ALLOCATION::REG_UNALLOCATED;

								//update back end RAT
								Back_End_RAT[ROB.entries[ROB.head].destArchReg].reg = ROB.entries[ROB.head].destReg;

								
								break;
							case Global::INSTRUCTION_TYPE::LOAD_TYPE:
							case Global::INSTRUCTION_TYPE::STORE_TYPE:
								ls2->run(Forward_Bus, Stalled_Stages, Memory_Array, ROB);
								Stalled_Stages[Global::STALLED_STAGE::LS2] = false;

								if (ROB.entries[ROB.head].type == Global::INSTRUCTION_TYPE::LOAD_TYPE)
								{
									//free up reg
									old_reg = Back_End_RAT[ROB.entries[ROB.head].destArchReg].reg;
									Register_File[old_reg].status = Global::REGISTER_ALLOCATION::REG_UNALLOCATED;

									//update back end RAT
									Back_End_RAT[ROB.entries[ROB.head].destArchReg].reg = ROB.entries[ROB.head].destReg;

									stats.loads_count++;
								}
								else
								{
									stats.stores_count++;
								}
								break;
							default:
								break;
							}

							if (ROB.entries[ROB.head].type != Global::INSTRUCTION_TYPE::STORE_TYPE)
							{
								//Mark register file as committed
								Register_File[ROB.entries[ROB.head].destReg].status = Global::REGISTER_ALLOCATION::ALLOC_COMMIT;
								Register_File[ROB.entries[ROB.head].destReg].value = ROB.entries[ROB.head].result;

								//Update Front-End Table
								if (Front_End_RAT[ROB.entries[ROB.head].destArchReg].reg == ROB.entries[ROB.head].destReg)
								{
									Front_End_RAT[ROB.entries[ROB.head].destArchReg].src_bit = Global::SOURCES::REGISTER_FILE;
								}
								
							}

							//update ROB head pointer
							ROB.head = (ROB.head + 1) % Global::ROB_SIZE;
						}
					}

					//run writeback
					//HALT = writeBack->run(Register_File, Forward_Bus, Back_End_RAT);
					HALT = writeBack->run(Forward_Bus, ROB, Register_File);

					if (!HALT)
					{
						//Run Multiply
						pipeline_struct_multiply = multiply->run(Forward_Bus, ROB, Stalled_Stages);

						//Run AL2
						pipeline_struct_alu2 = alu2->run(Forward_Bus, Stalled_Stages, ROB);

						//Run ALU1
						pipeline_struct_alu1 = alu1->run(Forward_Bus, Stalled_Stages);

						pipeline_struct_ls1 = ls1->run(Forward_Bus, Stalled_Stages, Memory_Array);

						//look into IQ
						pipeline_structs_iq = iq->run(Forward_Bus, Stalled_Stages);

						//Run Rename2/Dispatch
						pipeline_struct_dispatch = dispatch->run(Register_File, urf_size, Forward_Bus, Stalled_Stages, ROB, Front_End_RAT);

						//Decode/Rename1 handles delays based on OPCODE internally and update Stalled_Stages
						pipeline_struct_decode = decode->run(Register_File, urf_size, Forward_Bus, Stalled_Stages, ROB, Front_End_RAT);
						
						//as long as decode is not stalled and we still have data to read from the
						//input file, fetch the next command
						if ((!Stalled_Stages[Global::STALLED_STAGE::IQ]
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
							Global::Output("Input File Complete");
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
								Global::Output("-- Fetch Stalled -- ");

							if (!Stalled_Stages[Global::STALLED_STAGE::DECODE_RF])
								decode->display();
							else
								Global::Output("-- Decode/Rename1 Stalled -- " + decode->getInstruction());

							if (!Stalled_Stages[Global::STALLED_STAGE::DISPATCH])
								dispatch->display();
							else
							{
								Global::Output("-- Rename2/Dispatch Stalled -- " + dispatch->getInstruction());
								stats.dispatch_stall_count++;
							}

							if (!Stalled_Stages[Global::STALLED_STAGE::IQ])
								iq->display();
							else
								Global::Output("-- Issue Queue Stalled -- ");

							if (issued_instructions.size() > 0)
							{
								Global::Output("Issued Instructions - ");
								for (int x = 0; x < issued_instructions.size(); x++)
									Global::Output("  " + issued_instructions[x].untouched_instruction);
							}

							if (!Stalled_Stages[Global::STALLED_STAGE::ALU1])
								alu1->display();
							else
								Global::Output("-- ALU1 Stalled -- " + alu1->getInstruction());

							if (!Stalled_Stages[Global::STALLED_STAGE::ALU2])
								alu2->display();
							else
								Global::Output("-- ALU2 Stalled -- " + alu2->getInstruction());

							if (!Stalled_Stages[Global::STALLED_STAGE::BRANCH])
								branch->display();
							else
								Global::Output("-- Branch Stalled -- " + branch->getInstruction());

							if (!Stalled_Stages[Global::STALLED_STAGE::LS1])
								ls1->display();
							else
								Global::Output("-- Load/Store 1 Stalled -- " + ls1->getInstruction());

							if (!Stalled_Stages[Global::STALLED_STAGE::LS2])
								ls2->display();
							else
								Global::Output("-- Load/Store 2 Stalled -- " + ls2->getInstruction());

							writeBack->display();
						}
#pragma endregion DEBUG_OUTPUT

						//*************************
						//set up for the next cycle
						//*************************

						writeBack->setPipelineStruct(pipeline_struct_alu2);
						writeBack->setPipelineStruct(pipeline_struct_multiply);
						writeBack->setPipelineStruct(pipeline_struct_ls2);
						writeBack->setPipelineStruct(pipeline_struct_branch);

						//as long as we are not stalled, pass data
						if (!Stalled_Stages[Global::STALLED_STAGE::ALU1]
							&& !Stalled_Stages[Global::STALLED_STAGE::ALU2])
						{
							alu2->setPipelineStruct(pipeline_struct_alu1);
							pipeline_struct_alu1 = garbage_struct;
						}
						//if alu2 is not stalled, just pass in bubble
						else if (!Stalled_Stages[Global::STALLED_STAGE::ALU2])
						{
							alu2->setPipelineStruct(garbage_struct);
						}

						if (Stalled_Stages[Global::STALLED_STAGE::MULTIPLY] == false)
						{
							multiply->setPipelineStruct(garbage_struct);
						}

						if (Stalled_Stages[Global::STALLED_STAGE::LS2] == false)
						{
							ls2->setPipelineStruct(pipeline_struct_ls1);
							if ( pipeline_struct_ls1.pc_value != INT_MAX)
								Stalled_Stages[Global::STALLED_STAGE::LS2] = true;
						}

						//set up for issuing from IQ
						alu1->setPipelineStruct(garbage_struct);
						branch->setPipelineStruct(garbage_struct);
						if (Stalled_Stages[Global::STALLED_STAGE::LS1] == false)
						{
							ls1->setPipelineStruct(garbage_struct);
						}
						if (Stalled_Stages[Global::STALLED_STAGE::MULTIPLY] == false)
						{
							multiply->setPipelineStruct(garbage_struct);
						}

						if (pipeline_structs_iq.size() == 0)
						{
							stats.no_issue_count++;
						}

						for (int x = 0; x < pipeline_structs_iq.size(); x++)
						{
							Global::Output("");
							Global::Output("Issued Instructions");
							//based on opcode....
							switch (pipeline_structs_iq[x].instruction.op_code)
							{
								case Global::OPCODE::ADD:
								case Global::OPCODE::ADDL:
								case Global::OPCODE::AND:
								case Global::OPCODE::ANDL:
								case Global::OPCODE::EX_OR:
								case Global::OPCODE::EX_ORL:
								case Global::OPCODE::MOVC:
								case Global::OPCODE::OR:
								case Global::OPCODE::ORL:
								case Global::OPCODE::SUB:
								case Global::OPCODE::SUBL:
								case Global::OPCODE::HALT:
									alu1->setPipelineStruct(pipeline_structs_iq[x]);
									Global::Output("  " + pipeline_structs_iq[x].untouched_instruction);
									issued_instructions.push_back(pipeline_structs_iq[x]);
									break;
								case Global::OPCODE::BAL:
								case Global::OPCODE::BNZ:
								case Global::OPCODE::BZ:
								case Global::OPCODE::JUMP:
									branch->setPipelineStruct(pipeline_structs_iq[x]);
									Global::Output("  " + pipeline_structs_iq[x].untouched_instruction);
									issued_instructions.push_back(pipeline_structs_iq[x]);
									break;
								case Global::OPCODE::MUL:
								case Global::OPCODE::MULL:
									if (Stalled_Stages[Global::STALLED_STAGE::MULTIPLY] == false)
									{
										multiply->setPipelineStruct(pipeline_structs_iq[x]);
										Global::Output("  " + pipeline_structs_iq[x].untouched_instruction);
										issued_instructions.push_back(pipeline_structs_iq[x]);
									}
									break;
								case Global::OPCODE::LOAD:
								case Global::OPCODE::STORE:
									if (Stalled_Stages[Global::STALLED_STAGE::LS1] == false)
									{
										ls1->setPipelineStruct(pipeline_structs_iq[x]);
										Global::Output("  " + pipeline_structs_iq[x].untouched_instruction);
										issued_instructions.push_back(pipeline_structs_iq[x]);
									}
									break;
								default:
									break;
							}
						}

						iq->setPipelineStruct(pipeline_struct_dispatch);

						//if the IQ is not full, and we have not hit a HALT,
						//move the instruction from decode to dispatch
						if (!Stalled_Stages[Global::STALLED_STAGE::IQ]
							&& !HALT)
						{
							dispatch->setPipelineStruct(pipeline_struct_decode);
							pipeline_struct_decode = garbage_struct;
							PC++;
						}
						//or if we are stalled, then do nothing
						else if (!Stalled_Stages[Global::STALLED_STAGE::IQ] || HALT)
						{
							stats.dispatch_stall_count++;
						}

						//as long as decode is not stalled, and we are not updating
						// the PC from a branch, and we still have data to read
						//from fetch, move the fetch output to the input of decode
						if (!Stalled_Stages[Global::STALLED_STAGE::IQ]
							&& !HALT)
						{
							decode->setPipelineStruct(pipeline_struct_fetch);
							pipeline_struct_fetch = garbage_struct;
						}
						//or if we are not stalled, then feed in garbage (bubble)
						else if (!Stalled_Stages[Global::STALLED_STAGE::IQ] || HALT)
						{
							decode->setPipelineStruct(garbage_struct);
						}

						//see if we have any valid data to be
						//able to flush pipeline to completion
						if (decode->hasValidData()
							|| alu1->hasValidData()
							|| alu2->hasValidData()
							|| multiply->hasValidData()
							|| branch->hasValidData()
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

					issued_instructions.clear();

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

	fetch->closeFile();
	Global::closeFiles();

	//delete & free up memory
	delete fetch;
	delete decode;
	delete iq;
	delete alu1;
	delete alu2;
	delete multiply;
	delete branch;
	delete writeBack;

	return 0;
}

#pragma region "INITIALIZE FUNCTIONS" 
void initialize_forwarding_bus()
{
	for (int x = 0; x < Global::FORWARD_TYPE::FINAL_FORWARD_TYPE_TOTAL; x++)
	{
		Forward_Bus[x].flag = Global::FLAGS::CLEAR;
		Forward_Bus[x].opcode = Global::OPCODE::NONE;
		Forward_Bus[x].pc_value = -1;
		Forward_Bus[x].updatePC = false;
		Forward_Bus[x].target = INT_MAX;
		Forward_Bus[x].reg_info.tag = -1;
		Forward_Bus[x].reg_info.status = Global::REGISTER_ALLOCATION::REG_UNALLOCATED;
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
	for (int x = 0; x < urf_size + 1; x++)
	{
		Register_File[x].clear();
	}
}

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
	for (int x = 0; x < Global::ARCH_REGISTERS::FINAL_ARCH_REGISTERS_ITEM; x++)
	{
		Front_End_RAT[x].clear();
		Back_End_RAT[x].clear();
	}
}

void initialize_stats()
{
	stats.clear();
}

void initialize_pipeline()
{
	system("cls");

	cout << "-- Initializing Pipeline --" << endl;
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
	cout << "Initializing Rename Table Complete" << endl;
	cout << "Initializing Statistics" << endl
		<< "..." << endl;
	initialize_stats();
	cout << "Initializing Statistics Complete" << endl;
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
	pipeline_struct_dispatch = garbage_struct;
	pipeline_struct_alu1 = garbage_struct;
	pipeline_struct_alu2 = garbage_struct;
	pipeline_struct_branch = garbage_struct;
	pipeline_struct_ls1 = garbage_struct;
	pipeline_struct_ls2 = garbage_struct;
	decode->setPipelineStruct(garbage_struct);
	dispatch->setPipelineStruct(garbage_struct);
	iq->setPipelineStruct(garbage_struct);
	alu1->setPipelineStruct(garbage_struct);
	alu2->setPipelineStruct(garbage_struct);
	branch->setPipelineStruct(garbage_struct);
	ls1->setPipelineStruct(garbage_struct);
	ls2->setPipelineStruct(garbage_struct);
	writeBack->setPipelineStruct(garbage_struct);
	cout << "Resetting Pipeline Stages Complete" << endl;
	cout << "Resetting PC" << endl
		<< "..." << endl;
	PC = 4000;
	cout << "Resetting PC Complete" << endl;
	cout << "-- Initialization Complete --" << endl;
}
#pragma endregion INITIALIZE FUNCTIONS

#pragma region DISPLAY FUNCTIONS
void displayRegisterFile()
{
	Global::OutputURF("");
	Global::OutputURF("-- Unified Register File --");
	for (int x = 0; x < urf_size; x++)
	{
		Global::OutputURF("Register[U" + to_string(x) + "]:");
		Global::OutputURF("  status: " + Global::toString(Register_File[x].status));
		Global::OutputURF("  value : " + to_string(Register_File[x].value));
	}
}

void displayFRAT()
{
	Global::OutputFRAT("");
	Global::OutputFRAT("-- Front-End Rename Table --");
	for (int x = 0; x < Global::ARCH_REGISTERS::FINAL_ARCH_REGISTERS_ITEM; x++)
	{
		Global::OutputFRAT("Arch Reg: " + Global::toString(Global::ARCH_REGISTERS(x)));
		if (Front_End_RAT[x].src_bit == Global::SOURCES::ROB)
		{
			Global::OutputFRAT("  URF link : " + to_string(Front_End_RAT[x].reg));
			Global::OutputFRAT("  ROB Loc  : " + to_string(Front_End_RAT[x].rob_loc));
			
		}
		Global::OutputFRAT("  src bit  : " + Global::toString(Front_End_RAT[x].src_bit));
	}

	string cmd = "notepad.exe " + Global::getOutFileFRAT();
	system(cmd.c_str());
}

void displayRRAT()
{
	Global::OutputBRAT("");
	Global::OutputBRAT("-- Back-End Rename Table --");
	for (int x = 0; x < Global::ARCH_REGISTERS::FINAL_ARCH_REGISTERS_ITEM; x++)
	{
		Global::OutputBRAT("Arch Reg: " + Global::toString(Global::ARCH_REGISTERS(x)));
		if (Back_End_RAT[x].reg != -1)
		{
			Global::OutputBRAT("  URF link : " + to_string(Back_End_RAT[x].reg));
		}
		else
		{
			Global::OutputBRAT("  Not Applicable");
		}
	}

	string cmd = "notepad.exe " + Global::getOutFileBRAT();
	system(cmd.c_str());
}

void displayMemory(int a1, int a2)
{
	Global::Output("");
	Global::Output("-- Memory Locations --");

	for (int x = a1; x <= a2; x++)
	{
		Global::Output("Memory[" + to_string(x) + "] = " + to_string(Memory_Array[x]));
	}
}

void displayStalledStages()
{
	Global::Output("");
	Global::Output("-- Stalled stages --");
	Global::Output("Fetch		: " + to_string(Stalled_Stages[Global::STALLED_STAGE::FETCH]));
	Global::Output("DECODE/RENAME1	: " + to_string(Stalled_Stages[Global::STALLED_STAGE::DECODE_RF]));
	Global::Output("RENAME2/DISPATCH : " + to_string(Stalled_Stages[Global::STALLED_STAGE::DISPATCH]));
	Global::Output("ALU1		    : " + to_string(Stalled_Stages[Global::STALLED_STAGE::ALU1]));
	Global::Output("ALU2		    : " + to_string(Stalled_Stages[Global::STALLED_STAGE::ALU2]));
	Global::Output("BRANCH		: " + to_string(Stalled_Stages[Global::STALLED_STAGE::BRANCH]));
	Global::Output("WRITEBACK	: " + to_string(Stalled_Stages[Global::STALLED_STAGE::WRITEBACK]));
	Global::Output("");
}

void displayROB()
{
	Global::OutputROB("");
	Global::OutputROB("-- ROB Information --");
	Global::OutputROB(" HEAD: " + to_string(ROB.head));
	Global::OutputROB(" TAIL: " + to_string(ROB.tail));
	for (int x = ROB.head; x != ROB.tail + 1; x++)
	{
		Global::OutputROB("   Entry[" + to_string(x) + "] = ");
		Global::OutputROB("      PC Value  : " + to_string(ROB.entries[x].pc_value));
		Global::OutputROB("      Instruct  : " + ROB.entries[x].instruction);
		Global::OutputROB("      Allocation: " + Global::toString(ROB.entries[x].alloc));
		Global::OutputROB("      Instr Type: " + Global::toString(ROB.entries[x].type));
		Global::OutputROB("      Arch Reg  : " + Global::toString(ROB.entries[x].destArchReg));
		Global::OutputROB("      URF Reg   : " + to_string(ROB.entries[x].destReg));
		Global::OutputROB("      Flags     : " + Global::toString(ROB.entries[x].flags));
		Global::OutputROB("      Result    : " + to_string(ROB.entries[x].result));

		if (x == (Global::ROB_SIZE - 1))
		{
			x = 0;
		}
	}

	string cmd = "notepad.exe " + Global::getOutFileROB();
	system(cmd.c_str());
}

void displayStats()
{
	stats.ipc = double(writeBack->instructionsComplete()) / double(iteration);
	Global::Output("");
	Global::Output("-- Execution Statistics--");
	Global::Output("  IPC realized                            : " + to_string(stats.ipc));
	Global::Output("  # of cycles which dispatched as stalled : " + to_string(stats.dispatch_stall_count));
	Global::Output("  # of cycles w/o issue                   : " + to_string(stats.no_issue_count));
	Global::Output("  # LOADs committed                       : " + to_string(stats.loads_count));
	Global::Output("  # STOREs committed                      : " + to_string(stats.stores_count));
}
#pragma endregion DISPLAY FUNCTIONS

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

	bool is_open = false;

	system("cls");

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
		outfile_name.append("_" + to_string(ltm.tm_hour) + "-" + to_string(ltm.tm_min) + "-" + to_string(ltm.tm_sec) + "]");
		cout << "File name = " + outfile_name + ".txt" << endl;
	}
	else
	{
		outfile_name = trim(outfile_name);
	}

	Global::setOutFiles(outfile_name + ".txt", 
					    outfile_name + +"_ROB.txt", 
						outfile_name + "_FRAT.txt", 
						outfile_name + "_BRAT.txt",
						outfile_name + "_URF.txt",
						outfile_name + "_MEM.txt");
}