#include "pch.h"
#include "Function.h"
#include "Error.h"
#include "Node.h"
#include "ScriptCore.h"

Value ArithPlus(void*, const Value& lhs, const Value& rhs)				{ return lhs + rhs; }

Value ArithMinus(void*, const Value& lhs, const Value& rhs)				{ return lhs - rhs; }

Value ArithMult(void*, const Value& lhs, const Value& rhs)				{ return lhs * rhs; }

Value ArithDiv(void*, const Value& lhs, const Value& rhs)				{ return lhs / rhs; }

//Value ArithPlusAs(const Value& lhs, const Value& rhs)					{ return lhs += rhs; }
//
//Value ArithMinusAs(const Value& lhs, const Value& rhs)				{ return lhs -= rhs; }
//
Value ArithMultAs(void*, Value& lhs, const Value& rhs)					{ return lhs = lhs * rhs; }
//
//Value ArithDivAs(const Value& lhs, const Value& rhs)					{ return lhs /= rhs; }

Value BooleanEqual(void*, const Value& lhs, const Value& rhs)			{ return lhs == rhs; }

Value BooleanNot(void*, const Value& rhs)								{ return !rhs; }

Value BooleanNotEqual(void*, const Value& lhs, const Value& rhs)		{ return !(lhs == rhs); }

Value BooleanLess(void*, const Value& lhs, const Value& rhs)			{ return lhs < rhs; }

Value BooleanGreater(void*, const Value& lhs, const Value& rhs)			{ return lhs > rhs; }

Value BooleanLessEqual(void*, const Value& lhs, const Value& rhs)		{ return lhs < rhs || lhs == rhs; }

Value BooleanGreaterEqual(void*, const Value& lhs, const Value& rhs)	{ return lhs > rhs || lhs == rhs; }

std::unordered_map<String, NativeFuncStorage> nativeFuncs = {
{"global", NativeFuncStorage {
	{"print", new Function<const Value>(1, [](void*, auto& v) {
		if (v.type() == EVT::Null) {
			std::cout << ">> Undefined\n";
			return Value{};
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
	{"+", new Function<const Value, const Value>(2, &ArithPlus)},
	{"-", new Function<const Value, const Value>(2, &ArithMinus)},
	{"*", new Function<const Value, const Value>(2, &ArithMult)},
	{"*", new Function<Value, const Value>(2, &ArithMultAs)},
	{"/", new Function<const Value, const Value>(2, &ArithDiv)},
	{"==", new Function<const Value, const Value>(2, &BooleanEqual)},
	{"!", new Function<const Value>(1, &BooleanNot)},
	{"!=", new Function<const Value, const Value>(2, &BooleanNotEqual)},
	{"<", new Function<const Value, const Value>(2, &BooleanLess)},
	{">", new Function<const Value, const Value>(2, &BooleanGreater)},
	{"<=", new Function<const Value, const Value>(2, &BooleanLessEqual)},
	{">=", new Function<const Value, const Value>(2, &BooleanGreaterEqual)},
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
	{"init", new Function<const Value, const Value>(2, [](void*, auto c, auto v) {
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
//static int depth = 0;
//static void* firstPtr = nullptr;
//static void* lastPtr = nullptr;
void ScriptFunction::invoke(Value& value)
{
	/*depth++;
	int here = 1;
	if (!firstPtr) firstPtr = &here;
	printf("Pointer: %p | Distance: %lu | Total: %lu\n", (void*)&here, (unsigned long)lastPtr - (unsigned long)&here, (unsigned long)firstPtr - (unsigned long)&here);
	lastPtr = &here;
	printf("%d\n", depth);*/
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
