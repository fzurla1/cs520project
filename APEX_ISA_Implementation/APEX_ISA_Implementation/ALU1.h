/*
 *	ALU1 Class
 *	This class is responsible for gathering instructions from Decode/RF
 *	for the ALU functonal unit and passing it to the ALU2 functional unit
 *  for execution.
 */

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
	Global::apexStruct run(	Global::Forwarding_Info(&Forward_Bus)[Global::FINAL_FORWARD_TYPE_TOTAL],
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
};

#endif //ALU1_H