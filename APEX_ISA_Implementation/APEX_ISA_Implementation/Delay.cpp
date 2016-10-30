#include "stdafx.h"
#include "Delay.h"

Delay::Delay()
{
}


Delay::~Delay()
{
}

Global::apexStruct Delay::run(Global::apexStruct input_struct)
{
	Global::apexStruct output_struct = input_struct;
	return output_struct;
}

bool Delay::isStalled()
{
	return stalled;
}

void Delay::display()
{

}
