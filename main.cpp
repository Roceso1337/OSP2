//Daniel Sze
//Robert Carneiro
#include "main.h"

int main(int argc, char *argv[])
{
	if(argc < 1) err("Invalid arguments\n");

    return 0;
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
