#include "pch.h"
#include "Function.h"
#include "Error.h"
#include "Node.h"
#include "ScriptCore.h"

Value ArithPlus(Value lhs, Value rhs)
{
	return lhs + rhs;
}

Value ArithMinus(Value lhs, Value rhs)
{
	return lhs - rhs;
}

Value Assign(Value lhs, Value rhs)
{
	return lhs - rhs;
}

NativeFuncStorage nativeFuncs = {
	{"print", new Function<Value>(1, [](auto v) {
		if (v.type == EVT::Null) {
			std::cout << ">> Undefined\n";
			return Value();
	}
		std::cout << ">> " << v.GetValue<String>() << '\n';
		return Value();
	})},
	{"time", new Function<>(0, []() {
		float tim = (float)time(NULL);
		return Value(EVT::Float, std::to_string(tim));
	})},
	{"+", new Function<Value, Value>(2, ArithPlus)},
	{"-", new Function<Value, Value>(2, ArithMinus)},
};

GlobalFuncStorage globalFuncs;
std::unordered_map<String, NativeFuncStorage> ObjectFuncs;
std::unordered_map<String, VarStorage> ObjectVars;


int BaseFunction::GetParamCount(Context& c, const String& name)
{	
	if (c.object) {
		if (ObjectFuncs.find(c.considerValue) == ObjectFuncs.end()) {

		}
		else {
			if (ObjectFuncs[c.considerValue].find(name) == ObjectFuncs[c.considerValue].end())
				error(("Function: " + name + " not found from object: " + c.considerValue).c_str());
			else
				return ObjectFuncs[c.considerValue].find(c.considerValue)->second->param_count;
		}
	}
	else {
		if (c.topLevel->functions.find(name) == c.topLevel->functions.end()) {
			if (globalFuncs.find(name) == globalFuncs.end()) {
				if (nativeFuncs.find(name) == nativeFuncs.end()) {
					error(("Function: " + name + " not found").c_str());
				}
				else return nativeFuncs.find(name)->second->param_count;
			}
			else return (int)globalFuncs.find(name)->second.params.size();
		}
		else return (int)c.topLevel->functions.find(name)->second.params.size();
	}
	return 0;
}

Value ScriptFunction::invoke()
{
	if (first) {
		Node* ptr = first;
		while (ptr != nullptr && !shouldReturn) {
			ptr->evaluate(this);
			ptr = ptr->next;
		}
		return returnValue;
	}
	else return Value();
}

ScriptFunction::~ScriptFunction()
{
	delete scope;
	delete first;
}
