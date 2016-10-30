#include "stdafx.h"
#include "Memory.h"

Memory::Memory()
{
}


Memory::~Memory()
{
}

Global::apexStruct Memory::run(Global::apexStruct input_struct, Global::Register_Info * forward_bus)
{
	Global::apexStruct output_struct = input_struct;
	return output_struct;
}

bool Memory::isStalled()
{
	return stalled;
}

void Memory::display()
{

}