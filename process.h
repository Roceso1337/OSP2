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
};

class process
{
public:
	std::vector<burst> bursts;
};

#endif