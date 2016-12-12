#include "memory.h"

const int memory::NEXTFIT=0;
const int memory::BESTFIT=1;
const int memory::WORSTFIT=2;

//pHistoryData
pHistoryData::pHistoryData()
{
	timeExecuted=-1;
	event=-1;
	processName='.';
}

pHistoryData::pHistoryData(int newTimeExecuted, int newEvent, char newPName)
{
	timeExecuted=newTimeExecuted;
	event=newEvent;
	processName=newPName;
}

//MEMORY
memory::memory()
{
	this->frameSize=32;
    this->memorySize=256;
    this->freeSpace=this->memorySize;
    this->mem=new char[this->memorySize];
    bzero(this->mem, this->memorySize);
}

memory::memory(int newFrameSize, int newMemorySize)
{
	this->frameSize=newFrameSize;
    this->memorySize=newMemorySize;
    this->freeSpace=this->memorySize;
    this->mem=new char[this->memorySize];
    bzero(this->mem, this->memorySize);
}

memory::~memory()
{
	//delete this->mem;
}

bool memory::isFinished(int cTime)
{
	int event=0;//doesnt matter but we need to pass it in
	process p;//doesnt matter but we need to pass it in
	bool success=nextEvent(cTime, event, p);
	return !success;
}

bool memory::nextEvent(int& cTime, int& eventFlag, process& p)//cTime = current time elapsed
{
	int nextTime=-1;
	eventFlag=-1;

	for(unsigned int i=0;i<processList.size();++i)
	{
		for(unsigned int j=0;j<processList[i].bursts.size();++j)
		{
			//go back far enough in the history but not too far
			int firstIndexAtTime=-1;
			for(int k=processHistory.size()-1;k>=0;--k)
			{
				if(processHistory[k].timeExecuted == cTime)
					firstIndexAtTime=k;
				else if(processHistory[k].timeExecuted < cTime)//no need to go further
					break;
			}

			//arrival time
			//process arrive
			if(processList[i].bursts[j].arrivalTime >= cTime)
			{
				//check if we already ran this process's burst
				if(firstIndexAtTime > -1)
				{
					bool alreadyRan=false;
					for(unsigned int k=firstIndexAtTime;k<processHistory.size();++k)
					{
						if((processHistory[k].processName == processList[i].processName)
							&& (processHistory[k].event == 0))
							alreadyRan=true;
					}

					if(alreadyRan) continue;//next process/burst
				}

				if(nextTime == -1)//first valid time we found
				{
					nextTime=processList[i].bursts[j].arrivalTime;
					p=processList[i];
					eventFlag=0;
				}
				else
				{
					if(processList[i].bursts[j].arrivalTime < nextTime)
					{
						nextTime=processList[i].bursts[j].arrivalTime;
						p=processList[i];
						eventFlag=0;
					}
				}
			}
			else
			{
				//check if we already ran this process
				if(firstIndexAtTime > -1)
				{
					bool alreadyRan=false;
					for(unsigned int k=firstIndexAtTime;k<processHistory.size();++k)
					{
						if((processHistory[k].processName == processList[i].processName)
							&& (processHistory[k].event == 1))
							alreadyRan=true;
					}

					if(alreadyRan) continue;//next process
				}

				//thread already arrived
				//maybe the duration of the process is almost over (process exit)
				if(processList[i].bursts[j].arrivalTime+processList[i].bursts[j].duration >= cTime)
				{
					if(nextTime == -1)//first valid time we found
					{
						nextTime=processList[i].bursts[j].arrivalTime+processList[i].bursts[j].duration;
						p=processList[i];
						eventFlag=1;
					}
					else
					{
						if(processList[i].bursts[j].arrivalTime+processList[i].bursts[j].duration < nextTime)
						{
							nextTime=processList[i].bursts[j].arrivalTime+processList[i].bursts[j].duration;
							p=processList[i];
							eventFlag=1;
						}
					}
				}
			}
		}
	}

	cTime=nextTime;

	return (nextTime > -1);//success?
}

int memory::getFreeSpace()
{
	return this->freeSpace;
}

bool memory::addProcess(const process& p, int algoFlag, int timeElapsed)
{
	bool success=false;

	bool before=true;//for next fit
	int cMemSize=-1;//for best/worst fit current best/worst case memory block size
	int bestIndex=-1;//for best/worst fit current best/worst case memory block index

	switch(algoFlag)
	{
		case memory::NEXTFIT:

			//each character of the memory
			for(int i=0;i<this->memorySize;++i)
			{
				if(this->freeSpace == this->memorySize)
				{
					memset(&this->mem[i], p.processName, p.memSize);
					this->freeSpace-=p.memSize;
					success=true;
					break;
				}
				else
				{
					if((this->mem[i] >= 0x41) && (this->mem[i] <= 0x5A))
					{
						//before
						if((before) && (i-p.memSize >= 0))
						{
							memset(&this->mem[i-p.memSize], p.processName, p.memSize);
							this->freeSpace-=p.memSize;
							success=true;
							break;
						}
						else before=false;
					}
					else
					{
						if(!before)
						{
							//after
							if(i+p.memSize <= this->memorySize)
							{
								memset(&this->mem[i], p.processName, p.memSize);
								this->freeSpace-=p.memSize;
								success=true;
								break;
							}
							else
								break;
						}
					}
				}
			}

			break;
		case memory::BESTFIT:

			//each line of the memory
			for(int i=0;i<this->memorySize/this->frameSize;++i)
			{
				//each frame of the memory
				for(int j=0;j<this->frameSize;++j)
				{
					int index=(i*this->frameSize)+j;
					if(!((this->mem[index] >= 0x41) && (this->mem[index] <= 0x5A))
						&& (index+p.memSize < this->memorySize))
					{
						//now check if theres enough space
						int k=0;//counter
						while(!((this->mem[index] >= 0x41) && (this->mem[index] <= 0x5A)))
						{
							if(k >= cMemSize) break;//memory is too big or below already available memory

							++k;
						}

						//we got the size of the empty mem block, lets check if its the one
						if(cMemSize == -1)
						{
							cMemSize=k;
							bestIndex=index;
						}
						else
						{
							if(k < cMemSize)
							{
								cMemSize=k;
								bestIndex=index;
							}
						}
					}
				}
			}

			//we made it! add it to the mem
			if(bestIndex > -1)
			{
				memset(&this->mem[bestIndex], p.processName, p.memSize);
				this->freeSpace-=p.memSize;
				success=true;
			}

			break;
		case memory::WORSTFIT:

			for(int i=0;i<this->memorySize/this->frameSize;++i)
			{
				//each frame of the memory
				for(int j=0;j<this->frameSize;++j)
				{
					int index=(i*this->frameSize)+j;
					if(!((this->mem[index] >= 0x41) && (this->mem[index] <= 0x5A))
						&& (index+p.memSize < this->memorySize))
					{
						//now check if theres enough space
						int k=0;//counter
						while(!((this->mem[index] >= 0x41) && (this->mem[index] <= 0x5A)))
						{
							if(k <= cMemSize) break;//memory is too small or below already available memory

							++k;
						}

						//we got the size of the empty mem block, lets check if its the one
						if(cMemSize == -1)
						{
							cMemSize=k;
							bestIndex=index;
						}
						else
						{
							if(k > cMemSize)
							{
								cMemSize=k;
								bestIndex=index;
							}
						}
					}
				}
			}

			//we made it! add it to the mem
			if(bestIndex > -1)
			{
				memset(&this->mem[bestIndex], p.processName, p.memSize);
				this->freeSpace-=p.memSize;
				success=true;
			}

			break;
	}

	if(success)
	{
		pHistoryData hist(timeElapsed, 0, p.processName);
		processHistory.push_back(hist);
	}

	return success;
}

void memory::removeProcess(const process& p, int timeElapsed)
{
	//each line of the memory
	for(int i=0;i<this->memorySize/this->frameSize;++i)
	{
		//each frame of the memory
		for(int j=0;j<this->frameSize;++j)
		{
			int index=(i*this->frameSize)+j;
			if(this->mem[index] == p.processName)
			{
				bzero(&this->mem[index], sizeof(char));
				++this->freeSpace;
			}
		}
	}

	pHistoryData hist(timeElapsed, 1, p.processName);
	processHistory.push_back(hist);
}

void memory::skip(const process& p, int timeElapsed)
{
	pHistoryData hist(timeElapsed, 0, p.processName);
	processHistory.push_back(hist);
}

int memory::defragment(int& timeElapsed)
{
	int bIndex=-1;

	//each character of the memory
	for(int i=0;i<this->memorySize;++i)
	{
		//is it filled?
		if((this->mem[i] >= 0x41) && (this->mem[i] <= 0x5A))
		{
			//move the memory
			memcpy(&this->mem[bIndex], &this->mem[i], this->memorySize-this->freeSpace);
			bzero(&this->mem[this->memorySize-this->freeSpace], this->freeSpace);
			timeElapsed+=this->memorySize-this->freeSpace;
			return this->memorySize-this->freeSpace;
		}
		else
		{
			//where is it
			if(bIndex == -1) bIndex=i;
		}
	}

	return 0;
}

void memory::print()
{
	//top border
	for(int i=0;i<this->frameSize;++i) std::cout<<"=";
	std::cout<<std::endl;
	
	//each line of the memory
	for(int i=0;i<this->memorySize/this->frameSize;++i)
	{
		//each frame of the memory
		for(int j=0;j<this->frameSize;++j)
		{
			int index=(i*this->frameSize)+j;
			if((this->mem[index] >= 0x41) && (this->mem[index] <= 0x5A))
				std::cout<<this->mem[index];
			else
				std::cout<<'.';
		}
		std::cout<<std::endl;
	}

	//bottom border
	for(int i=0;i<this->frameSize;++i) std::cout<<"=";
	std::cout<<std::endl;
}