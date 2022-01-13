#include "pch.h"
#include "ScriptCore.h"

int BaseFunction::GetParamCount(const String& name)
{
	if (globalFuncs.find(name) == globalFuncs.end()) error(("Function: " + name + "not found").c_str());
	else return globalFuncs.find(name)->second->param_count;
	return 0;
}

Value ScriptFunction::invoke() const
{
	if (first) return first->evaluate();
	else return Value();
}

ScriptFunction::~ScriptFunction()
{
	delete scope;
	delete first;
}
