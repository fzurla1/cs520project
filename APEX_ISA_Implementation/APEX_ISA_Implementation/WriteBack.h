#ifndef WRITE_BACK_H
#define WRITE_BACK_H

#include "Global.h"

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
		Global::Register_Info(&Register_File)[Global::ARCH_REGISTER_COUNT],
		Global::Forwarding_Info(&Forward_Bus)[Global::FORWARDING_BUSES],
		int(&Most_Recent_Reg)[Global::ARCH_REGISTER_COUNT]);

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
	///	displays current status of pipeline stage
	/// </summary>
	void display();

private:
	Global::apexStruct myStruct;
	Global::apexStruct snapshot_before;
};

#endif //WRITE_BACK_H
