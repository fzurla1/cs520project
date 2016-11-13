#ifndef WRITE_BACK_H
#define WRITE_BACK_H

#include "Global.h"

class WriteBack
{
public:
	WriteBack();
	~WriteBack();
	void run(Global::apexStruct input_struct, Global::Register_Info * forward_bus, Global::Register_Info * register_file, int * HALT);

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
	Global::apexStruct snapshot_before;
};

#endif //WRITE_BACK_H
