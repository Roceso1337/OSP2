#include "memory.h"

const int memory::NEXTFIT=0;
const int memory::BESTFIT=1;
const int memory::WORSTFIT=2;

memory::memory()
{
	this->frameSize=8;
    this->memorySize=256;
}

memory::memory(int newFrameSize, int newMemorySize)
{
	this->frameSize=newFrameSize;
    this->memorySize=newMemorySize;
}

void addProcess(int algoFlag)
{
	switch(algoFlag)
	{
		case NEXTFIT:
			break;
		case BESTFIT:
			break;
		case WORSTFIT:
			break;
		default:
            break;
	}
}

void defragment()
{
	//
}
