#include "pch.h"
#include "Export.h"

#include "defines.h"
#include <map>
#include <list>
#include <vector>

#include "Parser.h"
#include "Error.h"
#include "ScriptCore.h"


static std::map<uint, Script*> Scripts;
uint activeScript = 0;

unsigned __int32 CompileScript(const char* data)
{
	ClearError();
	uint off = 0;
	uint idx = Scripts.size() ? Scripts.rbegin()->first + 1 : 1;
	Scripts[idx] = new Script();
	Script* s = Scripts[idx];
	activeScript = idx;
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

void EvaluateScript(unsigned __int32 s)
{
	if (Scripts.find(s) == Scripts.end()) return;
	Scripts[s]->evaluate();
	Value::CleanArrays();
}

void AddWrapped(const char* scope, const char* name, BaseFunction* function)
{
	if (auto it = nativeFuncs[scope].find(name); it != nativeFuncs[scope].end()) {
		delete it->second;
	}
	nativeFuncs[scope][name] = function;
}

void CleanScript(unsigned __int32 script)
{
	if (Scripts.find(script) != Scripts.end()) delete Scripts[script];
	Scripts.erase(script);
	Value::CleanArrays();
}

void GetError(const char* error, size_t size)
{
	
}
