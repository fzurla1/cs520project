#ifndef DELAY_H
#define DELAY_H

#include "Global.h"

class Delay
{
public:
	Delay();
	~Delay();
	Global::apexStruct run(Global::apexStruct input_struct);

	/// <summary>
	///	identifies if this stage is stalled
	/// </summary>
	bool isStalled();

	/// <summary>
	///	displays current status of pipeline stage
	/// </summary>
	void display();

private:
	bool stalled = false;
};

#endif //DELAY_H