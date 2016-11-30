//Daniel Sze
//Robert Carneiro
#include "main.h"
#include "memory.h"

int main(int argc, char *argv[])
{
	if(argc < 2) err("Invalid arguments\n");

    std::string fname=argv[1];

    return 0;
}

void TBD(memory m)
{
    int timeElapsed=0;
    while(!m.isFinished(timeElapsed))
    {
        //get the next event
        int event=0;//0 for arrival, 1 for exit/end of duration
        m.nextEvent(timeElapsed, event);

        if(event == 0)
        {
            //add the process to the memory
        }
        else if(event == 1)
        {
            //remove the process from the memory
        }
    }
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
