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

//-----------FUNCTIONS---------//

#pragma region "INITIALIZE FUNCTIONS" 
void initialize_flags()
{
	for each (bool b in alu_flags)
	{
		alu_flags[b] = Global::FLAGS::NA;
	}
}

void initialize_forwarding_bus()
{
	Global::Register_Info init;
	init.reg_tag = Global::ARCH_REGISTERS::NA;
	init.reg_value = 0;

	for (int x = 0; x < Global::FORWARDING_BUSES; x++)
	{
		forward_bus[x] = init;
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
	Global::Register_Info init;
	init.reg_tag = Global::ARCH_REGISTERS::NA;
	init.reg_value = 0;

	for (int x = 0; x < Global::ARCH_REGISTER_COUNT; x++)
	{
		register_file[x] = init;
	}
}

void initialize_pipeline()
{
	initialize_flags();
	initialize_forwarding_bus();
	initialize_register_file();
	initialize_memory();
}
#pragma endregion

//MAIN PROGRAM
int _tmain(int argc, _TCHAR* argv[])
{
	Fetch * fetch = new Fetch();
	Decode * decode = new Decode();
	ALU1 * alu1 = new ALU1();
	ALU2 * alu2 = new ALU2();
	Branch * branch = new Branch();
	Delay * delay = new Delay();
	Memory * memory = new Memory();
	WriteBack * writeBack = new WriteBack();

	int HALT = 0;

	initialize_pipeline();

	while (1 && !HALT)
	{
		//start pipeline
		Global::apexStruct pipeline_struct = fetch->run(PC);
		decode->run(pipeline_struct, register_file, &HALT);

		if (HALT == 0) //HALT has not occured
		{
			alu1->run(pipeline_struct, forward_bus);
			alu2->run(pipeline_struct, forward_bus, alu_flags);
			branch->run(pipeline_struct, forward_bus, alu_flags);
			delay->run(pipeline_struct);
			memory->run(pipeline_struct, forward_bus, memory_array);
			writeBack->run(pipeline_struct, forward_bus, register_file);
		}
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