#ifndef _MEMORY
#define _MEMORY

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <cstring>
#include <vector>
#include <iostream>
#include "process.h"

class pHistoryData
{
public:
	int timeExecuted;
	int event;
	char processName;

	pHistoryData();
	pHistoryData(int, int, char);
};

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
	std::vector<pHistoryData> processHistory;

	memory();
	memory(int, int);
	~memory();

	bool isFinished(int);
	bool nextEvent(int&, int&, process&);

	void addProcess(const process&, int, int);
	void removeProcess(const process&, int);
	void defragment();

	void print();
};

#endif
