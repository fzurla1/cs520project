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

	bool stalledStage(Global::apexStruct current, bool(&Stalled_Stages)[Global::STALLED_STAGE::FINAL_STALLED_STAGE_TOTAL]);

	std::vector<Global::apexStruct> run(Global::Forwarding_Info(&Forward_Bus)[Global::FORWARD_TYPE::FINAL_FORWARD_TYPE_TOTAL],
		bool(&Stalled_Stages)[Global::STALLED_STAGE::FINAL_STALLED_STAGE_TOTAL]);

private:
	Global::apexStruct myStruct;
	Global::apexStruct snapshotBefore;
	Global::apexStruct snapshotAfter;
};
