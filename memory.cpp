#include "memory.h"

const int memory::NEXTFIT=0;
const int memory::BESTFIT=1;
const int memory::WORSTFIT=2;

memory::memory()
{
	this->frameSize=8;
    this->memorySize=256;
    this->mem=new char[this->memorySize];
}

memory::memory(int newFrameSize, int newMemorySize)
{
	this->frameSize=newFrameSize;
    this->memorySize=newMemorySize;
    this->mem=new char[this->memorySize];
}

memory::~memory()
{
	delete this->mem;
}

bool memory::isFinished(int cTime)
{
	return (nextEvent(cTime) == -1);
}

int memory::nextEvent(int cTime)//cTime=current time elapsed
{
	int nextTime=-1;

	return nextTime;
}

void addProcess(int algoFlag)
{
	switch(algoFlag)
	{
		case memory::NEXTFIT:
			break;
		case memory::BESTFIT:
			break;
		case memory::WORSTFIT:
			break;
		default:
			break;
	}
}

void defragment()
{
	//
}
