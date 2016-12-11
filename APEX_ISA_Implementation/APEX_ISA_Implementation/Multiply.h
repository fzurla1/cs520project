#ifndef MUL_H
#define MUL_H

#include "Global.h"

class Multiply
{
public:
	/// <summary>
	/// Multiply Contructor
	/// </summary>
	Multiply();

	/// <summary>
	/// Multiply Destructor
	/// </summary>
	~Multiply();

	/// <summary>
	///	This function will:
	///	1) Read the forward bus for missing register values
	///	2) update the Global::apexStruct for the ALU2 phase
	/// </summary>
	Global::apexStruct run(Global::Forwarding_Info(&Forward_Bus)[Global::FINAL_FORWARD_TYPE_TOTAL],
		Global::Reorder_Buffer(&ROB),
		bool(&Stalled_Stages)[Global::FINAL_STALLED_STAGE_TOTAL]);

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
	int count = 1;
};

#endif //MUL_H