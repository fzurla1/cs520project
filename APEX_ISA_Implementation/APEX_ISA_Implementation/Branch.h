#ifndef BRANCH_H
#define BRANCH_H

#include "Global.h"

class Branch
{
public:
	Branch();
	~Branch();
	Global::apexStruct run(Global::apexStruct input_struct, Global::Register_Info * forward_bus, bool * flags);

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

#endif //BRANCH_H