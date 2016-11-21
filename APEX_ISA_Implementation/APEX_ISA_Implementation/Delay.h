#ifndef DELAY_H
#define DELAY_H

#include "Global.h"

class Delay
{
public:
	Delay();
	~Delay();

	/// <summary>
	///	This function will:
	///	This function does nothing. if memory is stalled, it stalls
	/// other than that, it simply holds the data for the next round.
	/// </summary>
	Global::apexStruct run(bool (&Stalled_Stages)[Global::TOTAL_STAGES]);

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
};

#endif //DELAY_H