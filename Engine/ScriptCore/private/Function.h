#pragma once
#include "defines.h"
#include <map>
#include <functional>
#include <iostream>

#include "Value.h"

struct Scope;
struct Node;
struct BaseFunction;
struct ScriptFunction;

struct Variable 
{ 
	Variable() : value(nullptr), type(0), init(false) {}
	Variable(Value* v, uint8 c, bool i) : value(v), type(c), init(i) {}
	~Variable() { 
		delete value; 
	}
	Variable(Variable& old) : value(old.value),	type(old.type), init(old.init) {
		old.value = nullptr;
	}
	Variable(Variable&& old) noexcept : value(std::move(old.value)), type(std::move(old.type)), init(std::move(old.init)) {
		old.value = nullptr;
	}
	Variable& operator=(Variable& old) {
		if (this == &old)
			return *this;
		delete value;
		value = old.value;
		type = old.type;
		init = old.init;
		old.value = nullptr;
		return *this;
	}
	Variable& operator=(Variable&& old) noexcept {
		if (this == &old)
			return *this;
		delete value;
		value = old.value;
		type = old.type;
		init = old.init;
		old.value = nullptr;
		return *this;
	}
	Value* value;
	uint8 type; // 0 = normal, 1 = constant, 2 = static
	bool init;
};

typedef std::unordered_map<String, BaseFunction*> NativeFuncStorage;
typedef std::unordered_map<String, ScriptFunction> LocalFuncStorage;
typedef std::unordered_map<String, ScriptFunction> GlobalFuncStorage;
typedef std::unordered_map<String, Variable> VarStorage;

int GetParamCount(Context& c, const String& scope, const String& name);

struct ScriptFunction
{
	ScriptFunction() {
		scope = nullptr;
		first = nullptr;
		continueNode = nullptr;
		shouldReturn = false;
	}
	~ScriptFunction();
	VarStorage params;

	Scope* scope;

	Node* first;
	Node* continueNode;
	bool shouldReturn;
	Value returnValue;

	void invoke(Value& value);
};


extern std::unordered_map<String, NativeFuncStorage> nativeFuncs;
extern std::unordered_map<String, VarStorage> ObjectVars;
extern GlobalFuncStorage globalFuncs;