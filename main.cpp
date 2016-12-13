//Daniel Sze
//Robert Carneiro
#include "main.h"
#include "memory.h"

int main(int argc, char *argv[])
{
	if(argc < 2) err("Invalid arguments\n");

	std::string fname=argv[1];
	std::ifstream fd(fname.c_str());
	std::string line;
	std::vector<std::string> lines;
	memory m;

	if (fd != NULL){
		while (std::getline(fd, line)){
			lines.push_back(line);
		}

        parse(lines, m);

        fd.close();
	}


    /*testing parsing
    for (unsigned int i = 0; i < m.processList.size(); i++){
        std::cout << m.processList[i].processName << m.processList[i].memSize << std::endl;
        for (unsigned int j = 0; j < m.processList[i].bursts.size(); j++){
            std::cout << m.processList[i].bursts[j].arrivalTime << " " << m.processList[i].bursts[j].duration << std::endl;
        }
    }
    */

	for(int i=0;i<3;++i)
	{
		m.clear();
		TBD(m, i);
		std::cout<<std::endl;
	}

	return 0;
}

void parse(std::vector<std::string> lines, memory& m){
	bool first = true;
	//int numProcesses = 0;
	  
	for (unsigned int i = 0; i < lines.size(); ++i){
		if (lines[i][0] == '#')
			continue;
		if (lines[i].empty())
			continue;
		if (first){
			first = false;
			continue;
		}

		char* parseString = new char [lines[i].length()+1];
        std::strcpy(parseString, lines[i].c_str()); 

		char* splitText = strtok(parseString, " ");
		char procName = splitText[0];

        splitText = strtok(NULL, " ");
		int memSize = atoi(splitText);

		std::vector<std::string> paramList;
        splitText = strtok(NULL, " ");

		while (splitText != NULL){
			std::string convText(splitText);
			paramList.push_back(convText);
			splitText = strtok(NULL, " ");
		}

		process p(procName, memSize);
		
		for (unsigned int j = 0; j < paramList.size(); j++){
		   std::size_t index = paramList[j].find('/'); 
		   int arrival = atoi(paramList[j].substr(0, index).c_str());
		   int duration = atoi(paramList[j].substr(index+1).c_str());
		   burst b(arrival, duration);
		   p.bursts.push_back(b);
		}  
        
        m.processList.push_back(p);
	}
}

void TBD(memory m, int algoFlag)
{
	int timeElapsed=0;
	int defragTime=0;

	std::cout<<"time "<<(timeElapsed+defragTime)<<"ms: ";
	if(algoFlag == memory::NEXTFIT)
		std::cout<<"Simulator started (Contiguous -- Next-Fit)"<<std::endl;
	else if(algoFlag == memory::BESTFIT)
		std::cout<<"Simulator started (Contiguous -- Best-Fit)"<<std::endl;
	else if(algoFlag == memory::WORSTFIT)
		std::cout<<"Simulator started (Contiguous -- Worst-Fit)"<<std::endl;

	while(!m.isFinished(timeElapsed))
	{
		//get the next event
		int event=0;//0 for arrival, 1 for exit/end of duration
		process p;
		m.nextEvent(timeElapsed, event, p);

		if(event == 0)
		{
			std::cout<<"time "<<(timeElapsed+defragTime)<<"ms: ";
			std::cout<<"Process "<<p.processName<<" arrived (requires "<<p.memSize<<" frames)"<<std::endl;

			if(m.getFreeSpace() >= p.memSize)
			{
				//add the process to the memory
				bool sucess=m.addProcess(p, algoFlag, timeElapsed);

				if(sucess)
				{
					std::cout<<"time "<<(timeElapsed+defragTime)<<"ms: ";
					std::cout<<"Placed process "<<p.processName<<":"<<std::endl;
				}
				else
				{
					std::cout<<"time "<<(timeElapsed+defragTime)<<"ms: ";
					std::cout<<"Cannot place process "<<p.processName<<" -- starting defragmentation"<<std::endl;

					//defragment if it didnt fit
					std::vector<char> pList;
					int frames=m.defragment(pList);
					defragTime+=frames;
					std::sort(pList.begin(), pList.end());

					std::cout<<"time "<<(timeElapsed+defragTime)<<"ms: ";
					std::cout<<"Defragmentation complete (moved "<<frames<<" frames: ";
					for(unsigned int i=0;i<pList.size();++i)
					{
						std::cout<<pList[i];
						if(i < pList.size()-1) std::cout<<", ";
					}
					std::cout<<")"<<std::endl;
				}
			}
			else
			{
				m.skip(p, timeElapsed);

				std::cout<<"time "<<(timeElapsed+defragTime)<<"ms: ";
				std::cout<<"Cannot place process "<<p.processName<<" -- skipped!"<<std::endl;
			}
		}
		else if(event == 1)
		{
			//remove the process from the memory
			std::cout<<"time "<<(timeElapsed+defragTime)<<"ms: ";
			std::cout<<"Process "<<p.processName<<" removed:"<<std::endl;

			m.removeProcess(p, timeElapsed);
		}

		m.print();
	}

	std::cout<<"time "<<(timeElapsed+defragTime)<<"ms: ";
	if(algoFlag == memory::NEXTFIT)
		std::cout<<"Simulator ended (Contiguous -- Next-Fit)"<<std::endl;
	else if(algoFlag == memory::BESTFIT)
		std::cout<<"Simulator ended (Contiguous -- Best-Fit)"<<std::endl;
	else if(algoFlag == memory::WORSTFIT)
		std::cout<<"Simulator ended (Contiguous -- Worst-Fit)"<<std::endl;
}

std::string intTOstring(int number)
{
   std::stringstream ss;
   ss<<number;
   return ss.str();
}

std::string floatTOstring(float number)
{
   std::stringstream ss;
   ss<<number;
   return ss.str();
}

void err(const char *msg)
{
	fprintf(stderr, "%s", msg);
	exit(EXIT_FAILURE);
}
