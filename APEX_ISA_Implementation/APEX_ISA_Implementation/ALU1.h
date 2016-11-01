#ifndef ALU1_H
#define ALU1_H

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
	///	This function will:
	///	1) Read the forward bus for missing register values
	///	2) update the Global::apexStruct for the ALU2 phase
	/// </summary>
	Global::apexStruct run(Global::apexStruct input_struct, Global::Register_Info * forward_bus);

	/// <summary>
	///	identifies if this stage is stalled
	/// </summary>
	bool isStalled();

	/// <summary>
	///	displays current status of pipeline stage
	/// </summary>
	void display();

private:
	bool stalled = false;
	Global::apexStruct snapshot_before;
	Global::apexStruct snapshot_after;
};

#endif //ALU1_H