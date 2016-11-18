#pragma once

#include "Global.h"

class Execute
{
public:
	Execute();
	~Execute();
	void get(int * out_result, int * out_flags);
	void set(TYPE_INSTRUCTION in_instructions, int in_operands[3]);
	void run();

private:
	TYPE_INSTRUCTION instruction;
	int operands[3];
	int result;
	int flags[5];
};

