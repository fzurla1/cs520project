#include "stdafx.h"
#include "Fetch.h"

using namespace std;

Fetch::Fetch()
{
}


Fetch::~Fetch()
{
}

Global::apexStruct Fetch::run(int PC)
{
	Global::apexStruct output_struct;
	output_struct.pc_value = PC;
	return output_struct;
}

void Fetch::display()
{

}