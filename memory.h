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
    int freeSpace;
    int lastIndex;

    char* mem;

public:
	static const int NEXTFIT;
	static const int BESTFIT;
	static const int WORSTFIT;
    static const int NONCONTIG;

	std::vector<process> processList;
	std::vector<pHistoryData> processHistory;

	memory();
	memory(int, int);
	~memory();

	bool isFinished(int);
	bool nextEvent(int&, int&, process&);
	int getFreeSpace();

	bool addProcess(const process&, int, int);
	void removeProcess(const process&, int);
	void skip(const process&, int);
	int defragment(std::vector<char>&);

	void print();
	void clear();
};

#endif
