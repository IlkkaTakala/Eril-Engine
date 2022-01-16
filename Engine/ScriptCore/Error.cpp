#include "pch.h"
#include "Error.h"
#include <iostream>

static bool errorFlag = false;
static std::string errorText = "";

void error(const char* string, Context* c)
{
	std::cout << "Row " << std::to_string(c ? c->row : 0) << ": " << string << '\n';
	errorFlag = true;
}

void error(const String& string, Context* c)
{
	std::cout << "Row " << std::to_string(c ? c->row : 0) << ": " << string << '\n';
	errorFlag = true;
}

void warn(const char* string, Context* c)
{
	std::cout << "Row " << std::to_string(c->row) << ": " << string << '\n';
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
