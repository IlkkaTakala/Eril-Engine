#pragma once
#include "defines.h"

struct Script;
struct Node;

namespace Parser
{
	void FindFunctions(const char* data, Script* script);
};

