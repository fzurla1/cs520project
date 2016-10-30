#pragma once

#include "Global.h"

class WriteBack
{
public:
	WriteBack();
	~WriteBack();
	void run(apexStruct input_struct);

	/// <summary>
	///	displays current status of pipeline stage
	/// </summary>
	void display();

private:
};

