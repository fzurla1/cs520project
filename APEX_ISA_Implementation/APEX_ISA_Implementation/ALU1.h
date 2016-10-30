#pragma once

#include "Global.h"

class ALU1
{
public:
	/// <summary>
	/// ALU1 Contructor
	/// </summary>
	ALU1();

	/// <summary>
	/// ALU1 Destructor
	/// </summary>
	~ALU1();

	/// <summary>
	///	This function will:\n
	///	1) Read the forward bus for missing register values\n
	///	2) update the apexStruct for the ALU2 phase\n
	/// </summary>
	apexStruct run(apexStruct input_struct);

	/// <summary>
	///	displays current status of pipeline stage
	/// </summary>
	void display();

private:
};

