#pragma once
#include "defines.h"

struct Script;

void FindFunctions(const char* data, Script* script);
void FindVariables(const char* data, Script* script);

