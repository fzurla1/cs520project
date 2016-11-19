#ifndef DELAY_H
#define DELAY_H

#include "Global.h"

class Delay
{
public:
	Delay();
	~Delay();
	Global::apexStruct run(bool (&Stalled_Stages)[Global::TOTAL_STAGES]);

	void setPipelineStruct(Global::apexStruct input_struct);

	bool hasValidData();

	/// <summary>
	///	displays current status of pipeline stage
	/// </summary>
	void display();

private:
	Global::apexStruct myStruct;
};

#endif //DELAY_H