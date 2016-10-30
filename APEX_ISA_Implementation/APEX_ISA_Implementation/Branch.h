#pragma once

#include "Global.h"

class Branch
{
public:
	Branch();
	~Branch();
	apexStruct run(apexStruct input_struct);

	/// <summary>
	///	displays current status of pipeline stage
	/// </summary>
	void display();

private:
};

