#pragma once
#include "defines.h"

void error(const char* string = 0, Context* c = nullptr);
void warn(const char* string = 0, Context* c = nullptr);

bool isError();

const char* GetError();

void ClearError();