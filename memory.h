#ifndef _MEMORY
#define _MEMORY

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "process.h"

class memory
{
	static const int NEXTFIT;
	static const int BESTFIT;
	static const int WORSTFIT;

	int frameSize;
    int memorySize;

    std::vector<process> processList;

public:
	memory();
	memory(int, int);

	void addProcess(int);

	void defragment();
};

#endif