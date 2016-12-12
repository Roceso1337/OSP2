#ifndef _MAIN
#define _MAIN

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include "memory.h"

struct process;

void parsePhys(std::vector<std::string>&, memory&);
void parseVirtual(std::vector<std::string>&, std::vector<int>&);
void handleVirt(std::vector<int>&, int);
void optimal(std::vector<int>&, const int);
void lru(std::vector<int>&, const int);
void lfu(std::vector<int>&, const int);
void TBD(memory, int);
std::string intTOstring(int);
std::string floatTOstring(float);
void err(const char*);

#endif
