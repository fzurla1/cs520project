#include "stdafx.h"
#include "Execute.h"


Execute::Execute()
{
}


Execute::~Execute()
{
}

void Execute::get(int * out_result, int * out_flags)
{
	out_result = &result;
	out_flags = flags;
}

void Execute::set(TYPE_INSTRUCTION in_instruction, int in_operands[3])
{
	instruction = in_instruction;
	operands = in_operands;
}

void Execute::run()
{

}
