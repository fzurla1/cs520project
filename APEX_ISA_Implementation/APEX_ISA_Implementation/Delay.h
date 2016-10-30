#pragma once

#include "Global.h"

class Delay
{
public:
	Delay();
	~Delay();
	apexStruct run(apexStruct input_struct);

	/// <summary>
	///	displays current status of pipeline stage
	/// </summary>
	void display();

private:
};

