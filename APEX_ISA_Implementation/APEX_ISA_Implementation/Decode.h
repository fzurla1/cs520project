#ifndef DECODE_H
#define DECODE_H

#include "Global.h"

class Decode
{
public:
	Decode();
	~Decode();

	/// <summary>
	///	This function will:
	///	start to break down raw instruction string, grabbing data
	/// from the register file, but if the register is not valid, look
	/// to the forwarding bus for data. if data is still not valid, stall.
	/// </summary>
	Global::apexStruct run(
		Global::Register_Info(&Register_File)[Global::ARCH_REGISTER_COUNT],
		Global::Forwarding_Info(&Forward_Bus)[Global::FORWARDING_BUSES],
		bool(&Stalled_Stages)[Global::TOTAL_STAGES],
		int (&Most_Recent_Reg)[Global::ARCH_REGISTER_COUNT]);

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
	int last_alu_pc = INT_MAX;
};

#endif //DECODE_H