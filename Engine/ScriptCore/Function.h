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

typedef std::unordered_map<String, BaseFunction*> NativeFuncStorage;
typedef std::unordered_map<String, ScriptFunction> LocalFuncStorage;
typedef std::unordered_map<String, ScriptFunction> GlobalFuncStorage;
typedef std::unordered_map<String, Value*> VarStorage;

struct BaseFunction
{
	BaseFunction(int c) : param_count(c) {}
	virtual ~BaseFunction() {}
	static int GetParamCount(Context& c, const String& name);
	int param_count;
};

template<typename...Args>
struct Function : public BaseFunction
{
	typedef std::function<Value(const Args&...)> FuncParams;

	Function(const int count, FuncParams func) : BaseFunction(count), call(func) {}
	Function() : BaseFunction(0), call(nullptr) {}

	FuncParams call;

	void operator()(Value& value, const Args&... v) const {
		if (call) value = std::move(call(v...));
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


extern NativeFuncStorage nativeFuncs;
extern std::unordered_map<String, NativeFuncStorage> ObjectFuncs;
extern std::unordered_map<String, VarStorage> ObjectVars;
extern GlobalFuncStorage globalFuncs;