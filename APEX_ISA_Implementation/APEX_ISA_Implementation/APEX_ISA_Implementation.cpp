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

void initialize_memory()
{
	for (int x = 0; x < Global::MEMORY_SIZE; x++)
	{
		memory_array[x] = 0;
	}
}

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

	while (1)
	{
		//start pipeline
		Global::apexStruct pipeline_struct = fetch->run(PC);
		decode->run(pipeline_struct, register_file);
		alu1->run(pipeline_struct, forward_bus);
		alu2->run(pipeline_struct, forward_bus, alu_flags);
		branch->run(pipeline_struct, forward_bus, alu_flags);
		delay->run(pipeline_struct);
		memory->run(pipeline_struct, forward_bus, memory_array);
		writeBack->run(pipeline_struct, forward_bus, register_file);
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