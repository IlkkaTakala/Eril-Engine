#include "Error.h"
#include <iostream>

static bool errorFlag = false;
static std::string errorText = "";

void error(const String& string, Context* c)
{
	std::cout << "Line " << std::to_string(c ? c->row : 0) << ": " << string << '\n';
	errorText += string + '\n';
	errorFlag = true;
}

void warn(const String& string, Context* c)
{
	std::cout << "Line " << std::to_string(c ? c->row : 0) << ": " << string << '\n';
	errorText += string + '\n';
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
