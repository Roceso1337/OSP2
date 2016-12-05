#include "memory.h"

const int memory::NEXTFIT=0;
const int memory::BESTFIT=1;
const int memory::WORSTFIT=2;

memory::memory()
{
	this->frameSize=8;
    this->memorySize=256;
    this->mem=new char[this->memorySize];
    bzero(this->mem, this->memorySize);
}

memory::memory(int newFrameSize, int newMemorySize)
{
	this->frameSize=newFrameSize;
    this->memorySize=newMemorySize;
    this->mem=new char[this->memorySize];
    bzero(this->mem, this->memorySize);
}

memory::~memory()
{
	delete this->mem;
}

bool memory::isFinished(int cTime)
{
	int event=0;//doesnt matter but we need to pass it in
	process p;//doesnt matter but we need to pass it in
	bool success=nextEvent(cTime, event, p);
	return success;
}

bool memory::nextEvent(int& cTime, int& eventFlag, process& p)//cTime = current time elapsed
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
				{
					nextTime=processList[i].bursts[j].arrivalTime;
					p=processList[i];
				}
				else
				{
					if(processList[i].bursts[j].arrivalTime < nextTime)
					{
						nextTime=processList[i].bursts[j].arrivalTime;
						p=processList[i];
					}
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
					{
						nextTime=processList[i].bursts[j].arrivalTime+processList[i].bursts[j].duration;
						p=processList[i];
					}
					else
					{
						if(processList[i].bursts[j].arrivalTime+processList[i].bursts[j].duration < nextTime)
						{
							nextTime=processList[i].bursts[j].arrivalTime+processList[i].bursts[j].duration;
							p=processList[i];
						}
					}

					//process exit
					eventFlag=1;
				}
			}
		}
	}

	cTime=nextTime;

	return (nextTime > -1);//success?
}

void memory::addProcess(const process& p, int algoFlag)
{
	int cMemSize=-1;//for best/worst fit current best/worst case memory block size
	int bestIndex=-1;//for best/worst fit current best/worst case memory block index

	switch(algoFlag)
	{
		case memory::NEXTFIT:

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
						bool fits=true;
						for(int k=0;k<p.memSize;++k)
						{
							if((this->mem[index] >= 0x41) && (this->mem[index] <= 0x5A))
							{
								fits=false;
								break;
							}
						}

						//we made it! add it to the mem
						if(fits) memcpy(&this->mem[index], &p.processName, p.memSize);
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
			if(bestIndex > -1) memcpy(&this->mem[bestIndex], &p.processName, p.memSize);

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
			if(bestIndex > -1) memcpy(&this->mem[bestIndex], &p.processName, p.memSize);

			break;
	}
}

void memory::removeProcess(const process& p)
{
	//each line of the memory
	for(int i=0;i<this->memorySize/this->frameSize;++i)
	{
		//each frame of the memory
		for(int j=0;j<this->frameSize;++j)
		{
			int index=(i*this->frameSize)+j;
			if(this->mem[index] == p.processName)
				bzero(&this->mem[index], sizeof(char));
		}
		std::cout<<std::endl;
	}
}

void memory::defragment()
{
	//
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