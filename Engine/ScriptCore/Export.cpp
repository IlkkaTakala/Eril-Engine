#include "pch.h"
#include "Export.h"

#include "defines.h"
#include <map>
#include <list>
#include <vector>
#include <deque>
#include "Parser.h"
#include "Error.h"
#include "ScriptCore.h"

static std::unordered_map<String, int> ScriptNames;
static std::unordered_map<uint, Script*> Scripts;
static std::deque<uint> freeIdx;

uint lastIndex = 1;

unsigned long CompileScript(const char* data)
{
	ClearError();
	uint idx = 0;
	if (freeIdx.empty())
		idx = lastIndex++;
	else {
		idx = freeIdx.front();
		freeIdx.pop_front();
	}
	Scripts[idx] = new Script();
	Script* s = Scripts[idx];
	Parser::FindVariables(data, s);
	Parser::FindFunctions(data, s);

	if (isError()) {
		CleanScript(idx);
		return 0;
	}

	Value v;
	s->setup->invoke(v);
	delete s->setup;
	s->setup = nullptr;

	return idx;
}

unsigned long CompileScript(const char* name, const char* data)
{
	uint idx = CompileScript(data);
	ScriptNames.emplace(name, idx);
	return idx;
}

void EvaluateScript(unsigned long s)
{
	if (Scripts.find(s) == Scripts.end()) return;
	Scripts[s]->evaluate();
	Value::CleanArrays();
}

void EvaluateScript(const char* name)
{
	uint idx = 0;
	if (auto it(ScriptNames.find(name)); it != ScriptNames.end()) {
		idx = it->second;
		EvaluateScript(idx);
	}
}

void EvaluateAll()
{
	for (auto& [idx, s] : Scripts) {
		s->evaluate();
	}
	Value::CleanArrays();
}

void AddWrapped(const char* scope, const char* name, BaseFunction* function)
{
	if (auto it = nativeFuncs[scope].find(name); it != nativeFuncs[scope].end()) {
		delete it->second;
	}
	nativeFuncs[scope][name] = function;
}

void CleanScript(unsigned long script)
{
	if (Scripts.find(script) != Scripts.end()) delete Scripts[script];
	Scripts.erase(script);
	freeIdx.push_back(script);
	Value::CleanArrays();
}

void GetError(const char* error, size_t size)
{
	
}
