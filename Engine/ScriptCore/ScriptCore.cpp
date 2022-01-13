#include "pch.h"
#include "ScriptCore.h"

int BaseFunction::GetParamCount(const String& name)
{
	return globalFuncs[name]->param_count;
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
