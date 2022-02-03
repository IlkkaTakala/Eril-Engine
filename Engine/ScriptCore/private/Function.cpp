#include "Function.h"
#include "Error.h"
#include "Node.h"
#include "Scope.h"

Value ArithPlus(void*, const Value& lhs, const Value& rhs)				{ return lhs + rhs; }

Value ArithMinus(void*, const Value& lhs, const Value& rhs)				{ return lhs - rhs; }

Value ArithMult(void*, const Value& lhs, const Value& rhs)				{ return lhs * rhs; }

Value ArithDiv(void*, const Value& lhs, const Value& rhs)				{ return lhs / rhs; }

//Value ArithPlusAs(const Value& lhs, const Value& rhs)			{ return lhs += rhs; }
//
//Value ArithMinusAs(const Value& lhs, const Value& rhs)			{ return lhs -= rhs; }
//
//Value ArithMultAs(const Value& lhs, const Value& rhs)			{ return lhs *= rhs; }
//
//Value ArithDivAs(const Value& lhs, const Value& rhs)			{ return lhs /= rhs; }

bool BooleanEqual(void*, const Value& lhs, const Value& rhs)			{ return lhs == rhs; }

bool BooleanNot(void*, const Value& rhs)								{ return !rhs; }

bool BooleanNotEqual(void*, const Value& lhs, const Value& rhs)			{ return !(lhs == rhs); }

bool BooleanLess(void*, const Value& lhs, const Value& rhs)				{ return lhs < rhs; }

bool BooleanGreater(void*, const Value& lhs, const Value& rhs)			{ return lhs > rhs; }

bool BooleanLessEqual(void*, const Value& lhs, const Value& rhs)		{ return lhs < rhs || lhs == rhs; }

bool BooleanGreaterEqual(void*, const Value& lhs, const Value& rhs)		{ return lhs > rhs || lhs == rhs; }

std::unordered_map<String, NativeFuncStorage> nativeFuncs = {
{"global", NativeFuncStorage {
	{"print", new Function<void, Value>(1, [](void*, auto v) {
		if (v.type() == EVT::Null) {
			std::cout << ">> Undefined\n";
		}
		std::cout << ">> " << v.GetValue<String>() << '\n';
		return Value{};
	})},
	{"time", new Function<>(0, [](void*) {
		float tim = (float)time(NULL);
		return Value(EVT::Float, std::to_string(tim));
	})},
}},
{"op", NativeFuncStorage {
	{"+", new Function<Value, Value, Value>(2, &ArithPlus)},
	{"-", new Function<Value, Value, Value>(2, &ArithMinus)},
	{"*", new Function<Value, Value, Value>(2, &ArithMult)},
	{"/", new Function<Value, Value, Value>(2, &ArithDiv)},
	{"==", new Function<bool, Value, Value>(2, &BooleanEqual)},
	{"!", new Function<bool, Value>(1, &BooleanNot)},
	{"!=", new Function<bool, Value, Value>(2, &BooleanNotEqual)},
	{"<", new Function<bool, Value, Value>(2, &BooleanLess)},
	{">", new Function<bool, Value, Value>(2, &BooleanGreater)},
	{"<=", new Function<bool, Value, Value>(2, &BooleanLessEqual)},
	{">=", new Function<bool, Value, Value>(2, &BooleanGreaterEqual)},
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
}}
};

GlobalFuncStorage globalFuncs;
std::unordered_map<String, VarStorage> ObjectVars;


int GetParamCount(Context& c, const String& scope, const String& name)
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
				if (nativeFuncs.find(scope) == nativeFuncs.end() || nativeFuncs[scope].find(name) == nativeFuncs[scope].end()) {
					error(("Function: " + scope + "." + name + " not found").c_str(), &c);
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
