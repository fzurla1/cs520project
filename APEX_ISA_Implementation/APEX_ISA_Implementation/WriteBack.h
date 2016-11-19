#ifndef WRITE_BACK_H
#define WRITE_BACK_H

#include "Global.h"

class WriteBack
{
public:
	WriteBack();
	~WriteBack();
	void run(
		Global::Register_Info(&Register_File)[Global::ARCH_REGISTER_COUNT],
		Global::Forwarding_Info(&Forward_Bus)[Global::FORWARDING_BUSES],
		int(&Most_Recent_Reg)[Global::ARCH_REGISTER_COUNT],
		int * HALT);

	void setPipelineStruct(Global::apexStruct input_struct);
	
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
