#ifndef _PROCESS
#define _PROCESS

#include <stdio.h>
#include <stdlib.h>
#include <vector>

class burst
{
public:
	int arrivalTime;
	int duration;
public:
    burst(int arrival, int dur){ arrivalTime = arrival; duration = dur; }
};

class process
{
    int memSize;
    char processName;
public:
    process(char name, int size){ processName = name; memSize = size;}
	std::vector<burst> bursts;
};

#endif
