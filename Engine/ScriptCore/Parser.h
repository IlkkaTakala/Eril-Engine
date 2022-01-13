#pragma once
#include "defines.h"

struct Script;
struct Node;

namespace Parser
{
	int BeginParse(const char* data, uint off, uint& end, Node** nodePtr);

	void ParseFunctions(Script* script);

	void FindFunctions(const char* data, Script* script);
};

