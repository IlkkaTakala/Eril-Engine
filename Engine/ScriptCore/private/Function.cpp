#include "Function.h"
#include "Error.h"
#include "Node.h"
#include "Scope.h"

#include <iostream>

using namespace ScriptCore;

Value ArithPlus(void*, const Value& lhs, const Value& rhs)				{ return lhs + rhs; }

Value ArithMinus(void*, const Value& lhs, const Value& rhs)				{ return lhs - rhs; }

Value ArithMult(void*, const Value& lhs, const Value& rhs)				{ return lhs * rhs; }

Value ArithDiv(void*, const Value& lhs, const Value& rhs)				{ return lhs / rhs; }

Value ArithPlusAs(void*, Value& lhs, const Value& rhs)					{ return lhs = lhs + rhs; }

Value ArithMinusAs(void*, Value& lhs, const Value& rhs)					{ return lhs = lhs - rhs; }

Value ArithMultAs(void*, Value& lhs, const Value& rhs)					{ return lhs = lhs * rhs; }

Value ArithDivAs(void*, Value& lhs, const Value& rhs)					{ return lhs = lhs / rhs; }

bool BooleanEqual(void*, const Value& lhs, const Value& rhs)			{ return lhs == rhs; }

bool BooleanNot(void*, const Value& rhs)								{ return !rhs; }

bool BooleanNotEqual(void*, const Value& lhs, const Value& rhs)			{ return !(lhs == rhs); }

bool BooleanLess(void*, const Value& lhs, const Value& rhs)				{ return lhs < rhs; }

bool BooleanGreater(void*, const Value& lhs, const Value& rhs)			{ return lhs > rhs; }

bool BooleanLessEqual(void*, const Value& lhs, const Value& rhs)		{ return lhs < rhs || lhs == rhs; }

bool BooleanGreaterEqual(void*, const Value& lhs, const Value& rhs)		{ return lhs > rhs || lhs == rhs; }

std::unordered_map<String, NativeFuncStorage> MakeDefaults()
{
	return {
	{"global", NativeFuncStorage {
		{"print", new Function<Value>(1, [](void*, auto& v) {
			if (v.type() == EVT::Null) {
				std::cout << ">> Undefined\n";
			}
			std::cout << ">> " << (String)v << '\n';
			return Value{};
		})},
		{"time", new Function<>(0, [](void*) {
			float tim = (float)time(NULL);
			return Value(EVT::Float, std::to_string(tim));
		})},
	}},
	{"op", NativeFuncStorage {
		{"+", new Function<Value, Value>(2, &ArithPlus)},
		{"-", new Function<Value, Value>(2, &ArithMinus)},
		{"*", new Function<Value, Value>(2, &ArithMult)},
		{"+=", new Function<Value, Value>(2, &ArithPlusAs)},
		{"-=", new Function<Value, Value>(2, &ArithMinusAs)},
		{"*=", new Function<Value, Value>(2, &ArithMultAs)},
		{"/=", new Function<Value, Value>(2, &ArithDivAs)},
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
		{"type", new Function<>(0, [](void* val) {
			if (Variable* c = static_cast<Variable*>(val); c)
				return Value(typeName(c->value->type()));
			else return Value();
		})},
		{"unset", new Function<>(0, [](void* val) {
			if (Variable* c = static_cast<Variable*>(val); c) {
				*c->value = Value();
				c->init = false;
				return *c->value;
			}
			else return Value();
		})},
		{"copy", new Function<>(0, [](void* val) {
			if (Variable* c = static_cast<Variable*>(val); c) {
				return c->value->DeepCopy();
			}
			else return Value();
		})},
		{"size", new Function<>(0, [](void* val) {
			if (Variable* c = static_cast<Variable*>(val); c) {
				return Value(c->value->GetSize());
			}
			else return Value();
		})},
	}},
	{"array", NativeFuncStorage {
		{"init", new Function<Value, Value>(2, [](void*, auto c, auto v) {
			Value arr(EVT::Array, "");
			auto a = (Value::Array*)arr;
			if (a) {
				a->clear();
				a->resize((int)c);
				std::fill(a->begin(), a->end(), v);
			}
			return arr;
		})}
	}}
	};
}

std::unordered_map<String, NativeFuncStorage>& nativeFuncs()
{
	static std::unordered_map<String, NativeFuncStorage> funcs = MakeDefaults();
	return funcs;
}

GlobalFuncStorage globalFuncs;

int GetParamCount(Context& c, const String& scope, const String& name)
{	
	if (c.topLevel->functions.find(name) == c.topLevel->functions.end()) {
		if (globalFuncs.find(name) == globalFuncs.end()) {
			if (nativeFuncs().find(scope) == nativeFuncs().end() || nativeFuncs()[scope].find(name) == nativeFuncs()[scope].end()) {
				error(("Function: " + scope + "." + name + " not found").c_str(), &c);
				return 0;
			}
			else return nativeFuncs().find(scope)->second.find(name)->second->param_count;
		}
		else return (int)globalFuncs.find(name)->second.params.size();
	}
	else return (int)c.topLevel->functions.find(name)->second.params.size();
}

void ScriptFunction::invoke(Value& value)
{
	shouldReturn = false;
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
