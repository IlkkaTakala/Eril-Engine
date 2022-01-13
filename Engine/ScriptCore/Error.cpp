#include "pch.h"
#include "Error.h"
#include <iostream>

static bool errorFlag = false;
static std::string errorText = "";

void error(const char* string)
{
	std::cout << "Error while compiling: " << string << '\n';
	errorFlag = true;
}

bool isError()
{
	return errorFlag;
}

const char* GetError()
{
	return errorText.c_str();
}

void ClearError()
{
	errorFlag = false;
	errorText = "";
}
