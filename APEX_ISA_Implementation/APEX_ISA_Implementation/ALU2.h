#ifndef ALU2_H
#define ALU2_H

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
	/// This stage performs the following:
	/// 1) operates on the instruction
	/// 2) updates the apex struct
	/// 3) forwards data to the forwarding bus
	/// 4) updates the flags (zero, overflow, underflow, etc)
	/// </summary>
	Global::apexStruct run(Global::apexStruct input_struct, Global::Register_Info * forward_bus, bool * flags);

	/// <summary>
	///	displays current status of pipeline stage
	/// </summary>
	void display();

private:
	bool stalled = false;
	Global::apexStruct snapshot_before;
	Global::apexStruct snapshot_after;
};

#endif //ALU2_H