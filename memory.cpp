#include "memory.h"

const int memory::NEXTFIT=0;
const int memory::BESTFIT=1;
const int memory::WORSTFIT=2;
const int memory::NONCONTIG=3;

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
    this->lastIndex=0;
    this->mem=new char[this->memorySize];
    bzero(this->mem, this->memorySize);
}

memory::memory(int newFrameSize, int newMemorySize)
{
	this->frameSize=newFrameSize;
    this->memorySize=newMemorySize;
    this->freeSpace=this->memorySize;
    this->lastIndex=0;
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
				bool alreadyRan=false;
				if(firstIndexAtTime > -1)
				{
					for(unsigned int k=firstIndexAtTime;k<processHistory.size();++k)
					{
						if((processHistory[k].processName == processList[i].processName)
							&& ((processHistory[k].event == 0) || (processHistory[k].event == 2)))
							alreadyRan=true;
					}
				}

				//next process/burst
				if(!alreadyRan)
				{
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
			}

			//process already arrived
			//maybe the duration of the process is almost over (process exit)
			if(processList[i].bursts[j].arrivalTime+processList[i].bursts[j].duration >= cTime)
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
				
				if(nextTime == -1)//first valid time we found
				{
					for(unsigned int k=processHistory.size()-1;k>=0;--k)
					{
						if(processHistory[k].processName == processList[i].processName)
						{
							if(processHistory[k].event == 0)
							{
								nextTime=processList[i].bursts[j].arrivalTime+processList[i].bursts[j].duration;
								p=processList[i];
								eventFlag=1;
							}
							break;
						}
					}
				}
				else
				{
					if(processList[i].bursts[j].arrivalTime+processList[i].bursts[j].duration < nextTime)
					{
						for(unsigned int k=processHistory.size()-1;k>=0;--k)
						{
							if(processHistory[k].processName == processList[i].processName)
							{
								if(processHistory[k].event == 0)
								{
									nextTime=processList[i].bursts[j].arrivalTime+processList[i].bursts[j].duration;
									p=processList[i];
									eventFlag=1;
								}
								break;
							}
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

	int cMemSize=-1;//for best/worst fit current best/worst case memory block size
	int bestIndex=-1;//for best/worst fit current best/worst case memory block index

	switch(algoFlag)
	{
		case memory::NEXTFIT:

			if(this->lastIndex+p.memSize <= this->memorySize)
			{
				while((this->mem[this->lastIndex] >= 0x41) && (this->mem[this->lastIndex] <= 0x5A))
					++this->lastIndex;

				memset(&this->mem[this->lastIndex], p.processName, p.memSize);
				this->freeSpace-=p.memSize;
				this->lastIndex+=p.memSize;
				success=true;
			}
			else
			{
				//each character of the memory
				for(this->lastIndex=0;this->lastIndex<this->memorySize;++this->lastIndex)
				{
					if((this->mem[this->lastIndex] >= 0x41) && (this->mem[this->lastIndex] <= 0x5A))
					{
						if((this->lastIndex >= p.memSize) && (!success))
						{
							memset(&this->mem[this->lastIndex-p.memSize], p.processName, p.memSize);
							this->freeSpace-=p.memSize;
							this->lastIndex+=p.memSize;
							success=true;
						}
						break;
					}
				}

				if(!success)
				{
					while((this->lastIndex < this->memorySize)
						&& (this->mem[this->lastIndex] >= 0x41) && (this->mem[this->lastIndex] <= 0x5A))
						++this->lastIndex;

					if(this->lastIndex+p.memSize <= this->memorySize)
					{
						memset(&this->mem[this->lastIndex], p.processName, p.memSize);
						this->freeSpace-=p.memSize;
						this->lastIndex+=p.memSize;
						success=true;
					}
				}
			}

			break;
		case memory::BESTFIT:

			if(this->lastIndex+p.memSize <= this->memorySize)
			{
				while((this->mem[this->lastIndex] >= 0x41) && (this->mem[this->lastIndex] <= 0x5A))
					++this->lastIndex;

				while((this->lastIndex > 0)
					&& !((this->mem[this->lastIndex-1] >= 0x41) && (this->mem[this->lastIndex-1] <= 0x5A)))
					--this->lastIndex;

				memset(&this->mem[this->lastIndex], p.processName, p.memSize);
				this->freeSpace-=p.memSize;
				this->lastIndex+=p.memSize;
				success=true;
			}
			else
			{
				//each character of the memory
				for(this->lastIndex=0;this->lastIndex<this->memorySize;++this->lastIndex)
				{
					if((this->mem[this->lastIndex] >= 0x41) && (this->mem[this->lastIndex] <= 0x5A))
					{
						if((this->lastIndex >= p.memSize) && (!success))
						{
							memset(&this->mem[this->lastIndex-p.memSize], p.processName, p.memSize);
							this->freeSpace-=p.memSize;
							this->lastIndex+=p.memSize;
							success=true;
						}
						break;
					}
				}

				if(!success)
				{
					while((this->lastIndex < this->memorySize)
						&& (this->mem[this->lastIndex] >= 0x41) && (this->mem[this->lastIndex] <= 0x5A))
						++this->lastIndex;

					if(this->lastIndex+p.memSize <= this->memorySize)
					{
						memset(&this->mem[this->lastIndex], p.processName, p.memSize);
						this->freeSpace-=p.memSize;
						this->lastIndex+=p.memSize;
						success=true;
					}
				}
			}

			break;
		case memory::WORSTFIT:

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
					if(!((this->mem[i] >= 0x41) && (this->mem[i] <= 0x5A)))
					{
						if(i+p.memSize < this->memorySize)
						{
							//now check if theres enough space
							int k=0;//counter
							while(!((this->mem[k] >= 0x41) && (this->mem[k] <= 0x5A)))
								++k;

							//we got the size of the empty mem block, lets check if its the one
							if(cMemSize == -1)
							{
								cMemSize=k;
								bestIndex=i;
							}
							else
							{
								if(k > cMemSize)
								{
									cMemSize=k;
									bestIndex=i;
								}
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

		case memory::NONCONTIG:
            if (this->freeSpace >= p.memSize){
                int memCounter = p.memSize;
				for (int i = 0; i < this->memorySize;++i){
					if(!(this->mem[i] >= 0x41) && (this->mem[i] <= 0x5A)){
                        this->mem[i] = p.processName;
                        memCounter--;
                    }

                    if (memCounter <= 0){
                        success = true;
                        this->freeSpace-=p.memSize;    
                        break;
                    }
				}
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
	pHistoryData hist(timeElapsed, 2, p.processName);
	processHistory.push_back(hist);
}

int memory::defragment(std::vector<char>& pList)
{
	int bCounter=0;
	int frames=0;

	for(int i=0;i<this->memorySize;++i)
	{
		//is it filled?
		if((this->mem[i] >= 0x41) && (this->mem[i] <= 0x5A))
		{
			//which process is it
			for(unsigned int j=0;j<processList.size();++j)
			{
				if(this->mem[i] == processList[j].processName)
				{
					memset(&this->mem[i-bCounter], processList[j].processName, processList[j].memSize);
					bzero(&this->mem[i+(processList[j].memSize-bCounter)], bCounter);
					i+=(processList[j].memSize-bCounter-1);
					frames+=processList[j].memSize;
					pList.push_back(processList[j].processName);
					break;
				}
			}

			bCounter=0;
		}
		else
			++bCounter;
	}

	return frames;
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

void memory::clear()
{
    this->freeSpace=this->memorySize;
    bzero(this->mem, this->memorySize);
    this->lastIndex=0;
	processHistory.clear();
}
