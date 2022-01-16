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

FuncStorage globalFuncs = {
	{"print", new Function<Value>(1, [](auto v) {
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


int BaseFunction::GetParamCount(const String& name)
{
	if (globalFuncs.find(name) == globalFuncs.end()) error(("Function: " + name + " not found").c_str());
	else return globalFuncs.find(name)->second->param_count;
	return 0;
}

Value ScriptFunction::invoke() const
{
	if (first) {
		Node* ptr = first;
		while (ptr->next != nullptr) {
			ptr->evaluate();
			ptr = ptr->next;
		}
		return ptr->evaluate();
	}
	else return Value();
}

ScriptFunction::~ScriptFunction()
{
	delete scope;
	delete first;
}
