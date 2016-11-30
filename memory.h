#ifndef _MEMORY
#define _MEMORY

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "process.h"

class memory
{
	int frameSize;
    int memorySize;

    char* mem;

public:
	static const int NEXTFIT;
	static const int BESTFIT;
	static const int WORSTFIT;

	std::vector<process> processList;

	memory();
	memory(int, int);
	~memory();

	bool isFinished(int);
	void nextEvent(int&, int&);

	void addProcess(int);
	void defragment();
};

#endif
