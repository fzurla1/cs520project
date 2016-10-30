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

#include "Fetch.h"
#include "Decode.h"
#include "ALU1.h"
#include "ALU2.h"
#include "Branch.h"
#include "Delay.h"
#include "Memory.h"
#include "WriteBack.h"
#include "Global.h"

using namespace std;


int _tmain(int argc, _TCHAR* argv[])
{
	float * registers;
	string line;

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
		//do something
		fetch->run();
		decode->run();
		/*
		alu1->run(pipeline_struct);
		alu2->run(pipeline_struct);
		branch->run(pipeline_struct);
		delay->run(pipeline_struct);
		memory->run(pipeline_struct);
		writeBack->run(pipeline_struct);
		*/
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