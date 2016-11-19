#ifndef ALU1_H
#define ALU1_H

#include "Global.h"

class ALU1
{
public:
	/// <summary>
	/// ALU1 Contructor
	/// </summary>
	ALU1();

	/// <summary>
	/// ALU1 Destructor
	/// </summary>
	~ALU1();

	/// <summary>
	///	This function will:
	///	1) Read the forward bus for missing register values
	///	2) update the Global::apexStruct for the ALU2 phase
	/// </summary>
	Global::apexStruct run(	Global::Forwarding_Info(&Forward_Bus)[Global::FORWARDING_BUSES],
		bool(&Stalled_Stages)[Global::TOTAL_STAGES]);

	void setPipelineStruct(Global::apexStruct input_struct);

	bool hasValidData();

	/// <summary>
	///	displays current status of pipeline stage
	/// </summary>
	void display();

private:
	Global::apexStruct myStruct;
	Global::apexStruct snapshot_before;
	Global::apexStruct snapshot_after;
};

#endif //ALU1_H