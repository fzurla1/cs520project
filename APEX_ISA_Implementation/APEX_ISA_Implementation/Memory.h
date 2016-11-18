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
	Global::apexStruct run(
		Global::Forwarding_Info(&Forward_Bus)[Global::FORWARDING_BUSES],
		bool (&Stalled_Stages)[Global::TOTAL_STAGES],
		int (&Memory_Array)[Global::MEMORY_SIZE]);

	void setPipelineStruct(Global::apexStruct input_struct);

	/// <summary>
	///	displays current status of pipeline stage
	/// </summary>
	void display();

private:
	Global::apexStruct myStruct;
	Global::apexStruct snapshot_before;
	Global::apexStruct snapshot_after;
};

#endif //MEMORY_H