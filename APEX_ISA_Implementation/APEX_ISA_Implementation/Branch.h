#ifndef BRANCH_H
#define BRANCH_H

#include "Global.h"

class Branch
{
public:
	Branch();
	~Branch();
	Global::apexStruct run(
		int PC,
		Global::Forwarding_Info(&Forward_Bus)[Global::FORWARDING_BUSES],
		bool(&Stalled_Stages)[Global::TOTAL_STAGES],
		Global::Register_Info(&X));

	void setPipelineStruct(Global::apexStruct input_struct);

	bool hasValidData();

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