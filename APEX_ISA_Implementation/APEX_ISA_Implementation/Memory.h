#ifndef MEMORY_H
#define MEMORY_H

#include "Global.h"

class Memory
{
public:
	Memory();
	~Memory();

	/// <summary>
	/// The MEMORY stage will:
	/// 1) read forwarding data
	/// 2) get or set memory
	/// 3) forward results
	/// 4) update the output structure
	/// </summary>
	Global::apexStruct run(Global::apexStruct input_struct, Global::Register_Info * forward_bus, int * memory_array);

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

#endif //MEMORY_H