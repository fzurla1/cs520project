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
	Global::apexStruct run(
		Global::Forwarding_Info(&Forward_Bus)[Global::FINAL_FORWARD_TYPE_TOTAL],
		bool (&Stalled_Stages)[Global::FINAL_STALLED_STAGE_TOTAL],
		Global::Register_Info *Register_File,
		Global::Reorder_Buffer(&ROB));

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

#endif //ALU2_H