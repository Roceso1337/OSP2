//Daniel Sze / RCS: szed
//Robert Carneiro / RCS: carner5
#include "main.h"
#include "memory.h"

int main(int argc, char *argv[])
{
	if(argc != 3) err("Invalid arguments\n");

	std::string fname1=argv[1];
	std::ifstream fd(fname1.c_str());
	std::string line;
	std::vector<std::string> lines;
	memory m;

	if (fd != NULL){
		while (std::getline(fd, line)){
			lines.push_back(line);
		}

        std::sort(lines.begin(), lines.end());
		parsePhys(lines, m);

		for(int i=0;i<4;++i)
		{
			m.clear();
			letsgetphysical(m, i);
			std::cout<<std::endl;
		}

		fd.close();
	} else {
		err("Invalid file\n");
	}

	line.clear();
	lines.clear();
	std::string fname2=argv[2];
	fd.open(fname2.c_str());

	std::vector<int> virtualMem;

	if (fd != NULL){
		while (std::getline(fd, line)){
			lines.push_back(line);
		}

		parseVirtual(lines, virtualMem);
		handleVirt(virtualMem, 0);
		std::cout << std::endl;
		handleVirt(virtualMem, 1);
		std::cout << std::endl;
		handleVirt(virtualMem, 2);

		fd.close();
	} else {
		err("Invalid file\n");
	}

	return 0;
}

void parsePhys(std::vector<std::string>& lines, memory& m){
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
		delete(parseString);
	}
}

void parseVirtual(std::vector<std::string>& lines, std::vector<int>& virtualMem){
	for (unsigned int i = 0; i < lines.size(); ++i){
		char* parseString = new char [lines[i].length()+1];
		std::strcpy(parseString, lines[i].c_str()); 

		char* splitText = strtok(parseString, " ");
		virtualMem.push_back(atoi(splitText));
		splitText = strtok(NULL, " ");
		
		while (splitText != NULL){
			virtualMem.push_back(atoi(splitText));
			splitText = strtok(NULL, " ");
		}

		delete(parseString);
	}
}

void handleVirt(std::vector<int>& virtualMem, int algoFlag){
	/*
	 * 0 = Optimal
	 * 1 = LRU
	 * 2 = LFU
	 */
	const int F = 3;

	if (algoFlag == 0) {
		std::cout << "Simulating OPT with fixed frame size of " << F << std::endl;
		optimal(virtualMem, F);
	} else if (algoFlag == 1) {
		std::cout << "Simulating LRU with fixed frame size of " << F << std::endl;
		lru(virtualMem, F);
	} else if (algoFlag == 2) {
		std::cout << "Simulating LFU with fixed frame size of " << F << std::endl;
		lfu(virtualMem, F);
	}
}

void optimal(std::vector<int>& virtualMem, const int F){
	int mem[F];
    for (int i = 0; i < F; i++)
        mem[i] = -1;

	int pageFaults = 0;
	int memFill = 0;

	for (unsigned int i = 0; i < virtualMem.size(); i++){
		std::ostringstream oss;
		oss << "referencing page " <<  virtualMem[i];

		// case where there are unfilled spots
		if (memFill < F){
			bool filled = false;
			for (int j = 0; j < F; j++){
				if (mem[j] == virtualMem[i]){
					break;
				}
				if (mem[j] == -1){
					pageFaults++; 
					mem[j] = virtualMem[i];
					memFill++;
					oss << " [mem: ";
					for (int k = 0; k < F; k++){
						if (mem[k] == - 1)
							oss << ".";
						else
							oss << mem[k];
						if (k != F - 1)
							oss << " ";
					}
					oss <<  "] PAGE FAULT (no victim page)";
					filled = true;
				}

				if (filled) {
					std::cout << oss.str() << std::endl;
					break;
				}
			}
		} else { // all spots are already filled, begin OPT
			bool exists = false;

			// make sure the mem isnt already included
			for (int j = 0; j < F; j++){
				if (mem[j] == virtualMem[i]){
					exists = true;
					break;
				}
			}

			if (exists){
				continue;
			}

            int distances[F];
            for (int j = 0; j < F; j++)
                distances[j] = 2147483647;

			for (int j = 0; j < F; j++){ // look for replacement
				for (unsigned int k = i; k < virtualMem.size(); k++){
					if (mem[j] == virtualMem[k]){
						distances[j] = (int)(k) - i;
						break;
					}
				}
			}

			int min = 0;
			for (int j = 1; j < F; j++){
				if (distances[j] > distances[min] || (distances[j] == distances[min] && mem[j] < mem[min])){
					min = j;
				}
			}
			int fault = mem[min];
			mem[min] = virtualMem[i];

			oss << " [mem: ";
			for (int k = 0; k < F; k++){
				oss << mem[k];
				if (k != F - 1)
					oss << " ";
			}
			oss <<  "] PAGE FAULT (victim page " << fault << ")";
			std::cout << oss.str() << std::endl;

			pageFaults++;
		} 
	}

	std::cout << "End of OPT simulation (" << pageFaults << " page faults)" << std::endl;
}

void lru(std::vector<int>& virtualMem, const int F){
	int mem[F];
    for (int i = 0; i < F; i++)
        mem[i] = -1;

	int pageFaults = 0;
	int memFill = 0;

	for (unsigned int i = 0; i < virtualMem.size(); i++){
		std::ostringstream oss;
		oss << "referencing page " <<  virtualMem[i];

		// case where there are unfilled spots
		if (memFill < F){
			bool filled = false;
			for (int j = 0; j < F; j++){
				if (mem[j] == virtualMem[i]){
					break;
				}
				if (mem[j] == -1){
					pageFaults++; 
					mem[j] = virtualMem[i];
					memFill++;
					oss << " [mem: ";
					for (int k = 0; k < F; k++){
						if (mem[k] == - 1)
							oss << ".";
						else
							oss << mem[k];
						if (k != F - 1)
							oss << " ";
					}
					oss <<  "] PAGE FAULT (no victim page)";
					filled = true;
				}

				if (filled) {
					std::cout << oss.str() << std::endl;
					break;
				}
			}
		} else { // all spots are already filled, begin OPT
			bool exists = false;

			// make sure the mem isnt already included
			for (int j = 0; j < F; j++){
				if (mem[j] == virtualMem[i]){
					exists = true;
					break;
				}
			}

			if (exists){
				continue;
			}

            int distances[F];
            for (int j = 0; j < F; j++)
                distances[j] = 2147483647;

			for (int j = 0; j < F; j++){ // look for replacement
				for (unsigned int k = i; k >= 0; k--){
					if (mem[j] == virtualMem[k]){
						distances[j] = i - (int)k;
						break;
					}
				}
			}

			int min = 0;
			for (int j = 1; j < F; j++){
				if (distances[j] > distances[min] || (distances[j] == distances[min] && mem[j] < mem[min])){
					min = j;
				}
			}
			int fault = mem[min];
			mem[min] = virtualMem[i];

			oss << " [mem: ";
			for (int k = 0; k < F; k++){
				oss << mem[k];
				if (k != F - 1)
					oss << " ";
			}
			oss <<  "] PAGE FAULT (victim page " << fault << ")";
			std::cout << oss.str() << std::endl;

			pageFaults++;
		} 
	}

	std::cout << "End of LRU simulation (" << pageFaults << " page faults)" << std::endl;
}

void lfu(std::vector<int>& virtualMem, const int F){
	int mem[F];
    for (int i = 0; i < F; i++)
        mem[i] = -1;

	int pageFaults = 0;
	int memFill = 0;
	std::map<int, int> occurences;

	for (unsigned int i = 0; i < virtualMem.size(); i++){
		std::ostringstream oss;
		oss << "referencing page " <<  virtualMem[i];

		// case where there are unfilled spots
		if (memFill < F){
			bool filled = false;
			for (int j = 0; j < F; j++){
				if (mem[j] == virtualMem[i]){
					occurences[virtualMem[i]]++;
					break;
				}
				if (mem[j] == -1){
					pageFaults++; 
					mem[j] = virtualMem[i];
					memFill++;
					oss << " [mem: ";
					for (int k = 0; k < F; k++){
						if (mem[k] == - 1)
							oss << ".";
						else
							oss << mem[k];
						if (k != F - 1)
							oss << " ";
					}
					oss <<  "] PAGE FAULT (no victim page)";
					filled = true;
					occurences[mem[j]] =  1;
				}

				if (filled) {
					std::cout << oss.str() << std::endl;
					break;
				}
			}
		} else { // all spots are already filled, begin OPT

			bool exists = false;

			// make sure the mem isnt already included
			for (int j = 0; j < F; j++){
				if (mem[j] == virtualMem[i]){
					occurences[virtualMem[i]]++;
					exists = true;
					break;
				}
			}

			if (exists){
				continue;
			}

			int min = 0;
			for (int j = 1; j < F; j++){
				if (occurences[mem[j]] < occurences[mem[min]] || (occurences[mem[j]] == occurences[mem[min]]
							&& mem[j] < mem[min])){
					min = j;
				}
			}
			occurences[mem[min]] = 1;
			int fault = mem[min];
			mem[min] = virtualMem[i];
			occurences[mem[min]] = 1;

			oss << " [mem: ";
			for (int k = 0; k < F; k++){
				oss << mem[k];
				if (k != F - 1)
					oss << " ";
			}
			oss <<  "] PAGE FAULT (victim page " << fault << ")";
			std::cout << oss.str() << std::endl;

			pageFaults++;
		} 
	}

	std::cout << "End of LFU simulation (" << pageFaults << " page faults)" << std::endl;
}

void letsgetphysical(memory m, int algoFlag)
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
	else if(algoFlag == memory::NONCONTIG)
		std::cout<<"Simulator started (Non-contiguous)"<<std::endl;

	bool defrag = false;

	while(!m.isFinished(timeElapsed))
	{
		//get the next event
		int event=0;//0 for arrival, 1 for exit/end of duration
		process p;
		m.nextEvent(timeElapsed, event, p);

		if((event == 0) || (event == 2))
		{
			if(!defrag)
			{
				std::cout<<"time "<<(timeElapsed+defragTime)<<"ms: ";
				std::cout<<"Process "<<p.processName<<" arrived (requires "<<p.memSize<<" frames)"<<std::endl;
			}
			else
				defrag=false;

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

					defrag = true;
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
	else if(algoFlag == memory::NONCONTIG)
		std::cout<<"Simulator ended (Non-contiguous)"<<std::endl;
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
