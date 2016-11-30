//Daniel Sze
//Robert Carneiro
#include "main.h"

int main(int argc, char *argv[])
{
	if(argc < 2) err("Invalid arguments\n");

    std::string fname=argv[1];
    std::ifstream fd(fname.c_str());
    std::string line;
    std::vector<std::string> lines;
    memory mem();

    if (fd != NULL){
        while (std::getline(fd, line)){
            lines.push_back(line);
        }
    }

    return 0;
}

<<<<<<< HEAD
void parse(std::vector<std::string>& lines){
    bool first = true;
    int numProcesses = 0;
      
    for (unsigned int i = 0; i < lines.size(); i++){
        if (lines[i][0] == '#')
            continue;
        if (lines[i].empty())
            continue;
        if (first){
            numProcesses = atoi(lines[i].c_str());
            first = false;
            continue;
        }

    }
=======
void TBD()
{
    int timeElapsed=0;
>>>>>>> origin
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
