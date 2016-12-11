#pragma once

#include <vector>

#include "Global.h"

using namespace std;

class IssueQueue
{

public:
	IssueQueue();
	~IssueQueue();

	//IQ
	std::vector<Global::apexStruct> IQ;

	//Output vector to be dispatched to FU's
	std::vector<Global::apexStruct> output;

	void setPipelineStruct(Global::apexStruct inputStruct);

	void addIQEntry(std::vector<Global::apexStruct> &IQ, Global::apexStruct entry);

	void removeIQEntry(std::vector<Global::apexStruct> &IQ, int index);

	bool differentOpCode(Global::apexStruct current, std::vector<Global::apexStruct> output);
	
	bool stalledStage(Global::apexStruct current, bool(&Stalled_Stages)[Global::TOTAL_STAGES]);

	std::vector<Global::apexStruct> run(Global::Forwarding_Info(&Forward_Bus)[Global::FORWARDING_BUSES],
		bool(&Stalled_Stages)[Global::TOTAL_STAGES],
		int(&Most_Recent_Reg)[Global::ARCH_REGISTER_COUNT]);

private:
	Global::apexStruct myStruct;
	Global::apexStruct snapshotBefore;
	Global::apexStruct snapshotAfter;
};

