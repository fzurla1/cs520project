#ifndef WRITE_BACK_H
#define WRITE_BACK_H

#include "Global.h"
#include <vector>

class WriteBack
{
public:
	WriteBack();
	~WriteBack();

	/// <summary>
	///	This function will:
	///	write result to register file, forward bus, remove instruction from blocking register, 
	/// and state if we reached a HALt
	/// </summary>
	bool run(
		Global::Forwarding_Info(&Forward_Bus)[Global::FINAL_FORWARD_TYPE_TOTAL],
		Global::Reorder_Buffer(&ROB),
		Global::Register_Info * Register_File);

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
	string* getInstruction();

	/// <summary>
	///	displays current status of pipeline stage
	/// </summary>
	void display();

private:
	vector<Global::apexStruct> snapshot_before;
	vector<Global::apexStruct> myStructVector;
};

#endif //WRITE_BACK_H
