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
	Value* value;
	uint8 type; // 0 = normal, 1 = constant, 2 = static
	bool init;
};

typedef std::unordered_map<String, BaseFunction*> NativeFuncStorage;
typedef std::unordered_map<String, ScriptFunction> LocalFuncStorage;
typedef std::unordered_map<String, ScriptFunction> GlobalFuncStorage;
typedef std::unordered_map<String, Variable> VarStorage;

struct BaseFunction
{
	BaseFunction(int c) : param_count(c) {}
	virtual ~BaseFunction() {}
	static int GetParamCount(Context& c, const String& scope, const String& name);
	int param_count;
};

template<typename...Args>
struct Function : public BaseFunction
{
	typedef std::function<Value(void*, const Args&...)> FuncParams;

	Function(int count, FuncParams func) : BaseFunction(count), call(func) {}
	Function() : BaseFunction(0), call(nullptr) {}

	FuncParams call;

	void operator()(Value& value, void* ptr, const Args&... v) const {
		if (call) value = std::move(call(ptr, v...));
		else value = {};
	}

};

struct ScriptFunction
{
	ScriptFunction() {
		scope = nullptr;
		first = nullptr;
		continueNode = nullptr;
		shouldReturn = false;
		returnValue;
	}
	~ScriptFunction();
	std::vector<Value> params;
	std::map<String, int> param_names;

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