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
	int event=0;//doesnt matter but we need to pass it in
	nextEvent(cTime, event);
	return (cTime == -1);
}

void memory::nextEvent(int& cTime, int& eventFlag)//cTime = current time elapsed
{
	int nextTime=-1;
	eventFlag=-1;

	for(unsigned int i=0;i<processList.size();++i)
	{
		for(unsigned int j=0;j<processList[i].bursts.size();++j)
		{
			//arrival time
			//process arrive
			if(processList[i].bursts[j].arrivalTime >= cTime)
			{
				if(nextTime == -1)//first valid time we found
					nextTime=processList[i].bursts[j].arrivalTime;
				else
				{
					if(processList[i].bursts[j].arrivalTime < nextTime)
						nextTime=processList[i].bursts[j].arrivalTime;
				}

				//process arrival
				eventFlag=0;
			}
			else
			{
				//thread already arrived
				//maybe the duration of the process is almost over (process exit)
				if(processList[i].bursts[j].arrivalTime+processList[i].bursts[j].duration >= cTime)
				{
					if(nextTime == -1)//first valid time we found
						nextTime=processList[i].bursts[j].arrivalTime+processList[i].bursts[j].duration;
					else
					{
						if(processList[i].bursts[j].arrivalTime+processList[i].bursts[j].duration < nextTime)
							nextTime=processList[i].bursts[j].arrivalTime+processList[i].bursts[j].duration;
					}

					//process exit
					eventFlag=1;
				}
			}
		}
	}

	cTime=nextTime;
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
