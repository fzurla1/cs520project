#include "stdafx.h"
#include "WriteBack.h"

WriteBack::WriteBack()
{
}


WriteBack::~WriteBack()
{
}

void WriteBack::run(Global::apexStruct input_struct, Global::Register_Info * forward_bus, Global::Register_Info * register_file, int * HALT)
{
	snapshot_before = input_struct;
}

bool WriteBack::isStalled()
{
	return stalled;
}

void WriteBack::display()
{

}
