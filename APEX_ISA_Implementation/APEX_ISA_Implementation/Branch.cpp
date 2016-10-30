#include "stdafx.h"
#include "Branch.h"

Branch::Branch()
{
}


Branch::~Branch()
{
}

Global::apexStruct Branch::run(Global::apexStruct input_struct, Global::Register_Info * forward_bus, bool * flags)
{
	Global::apexStruct output_struct = input_struct;
	return output_struct;
}

bool Branch::isStalled()
{
	return stalled;
}

void Branch::display()
{

}
