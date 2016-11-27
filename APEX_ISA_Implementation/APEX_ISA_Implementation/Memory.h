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
		Global::Forwarding_Info(&Forward_Bus)[Global::FINAL_FORWARD_TYPE_TOTAL],
		bool (&Stalled_Stages)[Global::FINAL_STALLED_STAGE_TOTAL],
		int (&Memory_Array)[Global::MEMORY_SIZE]);

	/// <summary>
	///	This function will:
	///	set up the structure for the next run of the stage
	/// </summary>
	void setPipelineStruct(Global::apexStruct input_struct);

	/// <summary>
	///	This function will:
	///	return if the stage has valid data
	/// </summary>
	bool hasValidData();

	/// <summary>
	///	This function will:
	///	return the raw instruction string
	/// </summary>
	string getInstruction();

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