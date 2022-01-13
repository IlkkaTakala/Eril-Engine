#pragma once
#include <any>

unsigned __int32 CompileScript(const char* data);

void EvaluateScript(unsigned __int32 s);

void AddFuncs(std::any fun);

void CleanScript(unsigned __int32 script);

void GetError(const char* error, size_t size);