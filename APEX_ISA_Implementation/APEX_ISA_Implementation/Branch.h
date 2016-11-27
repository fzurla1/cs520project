#ifndef BRANCH_H
#define BRANCH_H

#include "Global.h"

class Branch
{
public:
	Branch();
	~Branch();

	/// <summary>
	///	This function will:
	///	if delay is not stalled, get forwarding info from ALU2 (as needed), 
	/// and if we branch, store X (as needed), and forwarding branching info
	/// to fetch
	/// </summary>
	Global::apexStruct run(
		int PC,
		Global::Forwarding_Info(&Forward_Bus)[Global::FINAL_FORWARD_TYPE_TOTAL],
		bool(&Stalled_Stages)[Global::FINAL_STALLED_STAGE_TOTAL],
		Global::Register_Info(&X));

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
	int x_value = 0;
	bool updatePC = false;
	int target = 0;
};

#endif //BRANCH_H