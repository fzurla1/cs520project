#ifndef MEMORY_H
#define MEMORY_H

#include "Global.h"

class Memory
{
public:
	Memory();
	~Memory();
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
};

#endif //MEMORY_H