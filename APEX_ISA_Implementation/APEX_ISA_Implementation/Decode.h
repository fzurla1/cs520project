#ifndef DECODE_H
#define DECODE_H

#include "Global.h"

class Decode
{
public:
	Decode();
	~Decode();
	Global::apexStruct run(Global::apexStruct input_struct, Global::Register_Info * register_file, int * HALT);

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

#endif //DECODE_H