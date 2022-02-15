#pragma once
#include "defines.h"

void error(const String& string, Context* c = nullptr);
void warn(const String& string = 0, Context* c = nullptr);

bool isError();

const char* GetError();

void ClearError();