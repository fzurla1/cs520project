#ifndef FETCH_H
#define FETCH_H

#include "Global.h"

class Fetch
{
public:
	Fetch();
	~Fetch();
	Global::apexStruct run(int(&PC),
		Global::Forwarding_Info(&Forward_Bus)[Global::FORWARDING_BUSES], 
		bool(&Stalled_Stages)[Global::TOTAL_STAGES]);
	void setFile(string filename);
	bool endOfFile();
	void display();

private:
	ifstream input_file;
	ifstream& GotoLine(ifstream& file, unsigned int num);
	Global::apexStruct snapshot_after;
};

#endif //FETCH_H