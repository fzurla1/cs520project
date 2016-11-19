#ifndef DECODE_H
#define DECODE_H

#include "Global.h"

class Decode
{
public:
	Decode();
	~Decode();
	Global::apexStruct run(
		Global::Register_Info(&Register_File)[Global::ARCH_REGISTER_COUNT],
		Global::Forwarding_Info(&Forward_Bus)[Global::FORWARDING_BUSES],
		bool(&Stalled_Stages)[Global::TOTAL_STAGES],
		int (&Most_Recent_Reg)[Global::ARCH_REGISTER_COUNT]);

	void setPipelineStruct(Global::apexStruct input_struct);

	bool hasValidData();

	/// <summary>
	///	displays current status of pipeline stage
	/// </summary>
	void display();

private:
	Global::apexStruct myStruct;
	Global::apexStruct snapshot_before;
	Global::apexStruct snapshot_after;
};

#endif //DECODE_H