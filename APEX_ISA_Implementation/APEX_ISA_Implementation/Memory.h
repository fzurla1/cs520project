#pragma once

#include "Global.h"

class Memory
{
public:
	Memory();
	~Memory();
	apexStruct run(apexStruct input_struct);

	/// <summary>
	///	displays current status of pipeline stage
	/// </summary>
	void display();

private:
};

