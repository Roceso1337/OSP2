#ifndef _MEMORY
#define _MEMORY

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <cstring>
#include <vector>
#include <iostream>
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
	void nextEvent(int&, int&, process&);

	void addProcess(const process&, int);
	void defragment();

	void print();
};

#endif
