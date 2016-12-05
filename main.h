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
#include "memory.h"

struct process;

void parse(std::vector<std::string>, memory&);
void TBD(memory, int);
std::string intTOstring(int);
std::string floatTOstring(float);
void err(const char*);

#endif
