#ifndef FETCH_H
#define FETCH_H

#include "Global.h"

class Fetch
{
public:
	Fetch();
	~Fetch();

	/// <summary>
	///	This function will:
	///	read the input file based on PC value, but if there is a
	/// branch, than use the updated PC value from branch instead
	/// stall if decode is stalled
	/// </summary>
	Global::apexStruct run(int(&PC),
		Global::Forwarding_Info(&Forward_Bus)[Global::FINAL_FORWARD_TYPE_TOTAL], 
		bool(&Stalled_Stages)[Global::FINAL_STALLED_STAGE_TOTAL]);

	/// <summary>
	///	This function will:
	///	set file name to read from
	/// </summary>
	void setFile(string filename);

	/// <summary>
	///	This function will:
	///	Return if the file is open and good
	/// </summary>
	bool fileIsGood();

	void closeFile();

	/// <summary>
	///	This function will:
	///	return if reached end of file
	/// </summary>
	bool endOfFile();

	/// <summary>
	///	This function will:
	///	set up the structure for the next run of the stage
	/// </summary>
	void setPipelineStruct(Global::apexStruct input_struct);

	/// <summary>
	///	displays current status of pipeline stage
	/// </summary>
	void display();

private:
	ifstream input_file;
	ifstream& GotoLine(ifstream& file, unsigned int num);
	Global::apexStruct snapshot_after;
	int last_pc = 0;
};

#endif //FETCH_H