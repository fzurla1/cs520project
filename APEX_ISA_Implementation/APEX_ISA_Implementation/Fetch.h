#ifndef FETCH_H
#define FETCH_H

#include "Global.h"

class Fetch
{
public:
	Fetch();
	~Fetch();
	Global::apexStruct run(int PC);
	void display();

private:
};

#endif //FETCH_H