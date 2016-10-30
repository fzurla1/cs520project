#pragma once

#include "Global.h"

class ALU2
{
public:
	/// <summary>
	/// ALU2 Constructor
	/// </summary>
	ALU2();

	/// <summary>
	/// ALU2 Destructor
	/// </summary>
	~ALU2();

	/// <summary>
	/// This stage performs the following:\n
	/// 1) operates on the instruction\n
	/// 2) updates the apex struct\n
	/// 3) forwards data to the forwarding bus\n
	/// 4) updates the flags (zero, overflow, underflow, etc)
	/// </summary>
	apexStruct run(apexStruct input_struct);

	/// <summary>
	///	displays current status of pipeline stage
	/// </summary>
	void display();

private:
};

