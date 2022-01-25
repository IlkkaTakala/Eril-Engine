#include "pch.h"
#include "Function.h"
#include "Error.h"
#include "Node.h"
#include "ScriptCore.h"

Value ArithPlus(const Value& lhs, const Value& rhs)				{ return lhs + rhs; }

Value ArithMinus(const Value& lhs, const Value& rhs)			{ return lhs - rhs; }

Value ArithMult(const Value& lhs, const Value& rhs)				{ return lhs * rhs; }

Value ArithDiv(const Value& lhs, const Value& rhs)				{ return lhs / rhs; }

//Value ArithPlusAs(const Value& lhs, const Value& rhs)			{ return lhs += rhs; }
//
//Value ArithMinusAs(const Value& lhs, const Value& rhs)			{ return lhs -= rhs; }
//
//Value ArithMultAs(const Value& lhs, const Value& rhs)			{ return lhs *= rhs; }
//
//Value ArithDivAs(const Value& lhs, const Value& rhs)			{ return lhs /= rhs; }

Value BooleanEqual(const Value& lhs, const Value& rhs)			{ return lhs == rhs; }

Value BooleanNot(const Value& rhs)								{ return !rhs; }

Value BooleanNotEqual(const Value& lhs, const Value& rhs)		{ return !(lhs == rhs); }

Value BooleanLess(const Value& lhs, const Value& rhs)			{ return lhs < rhs; }

Value BooleanGreater(const Value& lhs, const Value& rhs)		{ return lhs > rhs; }

Value BooleanLessEqual(const Value& lhs, const Value& rhs)		{ return lhs < rhs || lhs == rhs; }

Value BooleanGreaterEqual(const Value& lhs, const Value& rhs)	{ return lhs > rhs || lhs == rhs; }

std::unordered_map<String, NativeFuncStorage> nativeFuncs = {
{"global", NativeFuncStorage {
		{"print", new Function<Value>(1, [](auto v) {
		if (v.type() == EVT::Null) {
			std::cout << ">> Undefined\n";
			return Value{};
	}
		std::cout << ">> " << v.GetValue<String>() << '\n';
		return Value{};
	})},
	{"time", new Function<>(0, []() {
		float tim = (float)time(NULL);
		return Value(EVT::Float, std::to_string(tim));
	})},
}},
{"op", NativeFuncStorage {
	{"+", new Function<Value, Value>(2, &ArithPlus)},
	{"-", new Function<Value, Value>(2, &ArithMinus)},
	{"*", new Function<Value, Value>(2, &ArithMult)},
	{"/", new Function<Value, Value>(2, &ArithDiv)},
	{"==", new Function<Value, Value>(2, &BooleanEqual)},
	{"!", new Function<Value>(1, &BooleanNot)},
	{"!=", new Function<Value, Value>(2, &BooleanNotEqual)},
	{"<", new Function<Value, Value>(2, &BooleanLess)},
	{">", new Function<Value, Value>(2, &BooleanGreater)},
	{"<=", new Function<Value, Value>(2, &BooleanLessEqual)},
	{">=", new Function<Value, Value>(2, &BooleanGreaterEqual)},
}},
{"variable", NativeFuncStorage {
	{"type", new Function<Value>(1, [](auto v) {
		return Value((int)v.type());
	})}
}}
};

GlobalFuncStorage globalFuncs;
std::unordered_map<String, VarStorage> ObjectVars;


int BaseFunction::GetParamCount(Context& c, const String& scope, const String& name)
{	
	if (c.object) {
		/*if (ObjectFuncs[c.considerValue].find(name) == ObjectFuncs[c.considerValue].end())
			error(("Function: " + name + " not found from object: " + c.considerValue).c_str());
		else
			return ObjectFuncs[c.considerValue].find(c.considerValue)->second->param_count;*/
	}
	else {
		if (c.topLevel->functions.find(name) == c.topLevel->functions.end()) {
			if (globalFuncs.find(name) == globalFuncs.end()) {
				if (nativeFuncs.find(scope) == nativeFuncs.end() && nativeFuncs[scope].find(name) == nativeFuncs[scope].end()) {
					error(("Function: " + name + " not found").c_str());
				}
				else return nativeFuncs.find(scope)->second.find(name)->second->param_count;
			}
			else return (int)globalFuncs.find(name)->second.params.size();
		}
		else return (int)c.topLevel->functions.find(name)->second.params.size();
	}
	return 0;
}

void ScriptFunction::invoke(Value& value)
{
	if (first) {
		Node* ptr = first;
		while (ptr != nullptr && !shouldReturn) {
			ptr->evaluate(this, returnValue);
			ptr = ptr->next;
		}
		value = std::move(returnValue);
	}
	else value = {};
}

ScriptFunction::~ScriptFunction()
{
	delete scope;
	delete first;
}
