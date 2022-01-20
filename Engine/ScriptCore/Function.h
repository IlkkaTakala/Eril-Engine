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

typedef void(*FuncNoParam)();

typedef std::map<String, BaseFunction*> NativeFuncStorage;
typedef std::map<String, ScriptFunction> LocalFuncStorage;
typedef std::map<String, ScriptFunction> GlobalFuncStorage;

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
	typedef std::function<Value(Args...)> FuncParams;

	Function(const int count, FuncParams func) : BaseFunction(count), call(func) {}
	Function() : BaseFunction(0), call(nullptr) {}

	FuncParams call;

	Value operator()(Args... v) const {
		if (call) return call(v...);
		else return Value();
	}

};

struct ScriptFunction
{
	ScriptFunction() {
		scope = nullptr;
		first = nullptr;
	}
	~ScriptFunction();
	std::unordered_map<String, Node*> params;

	Scope* scope;

	Node* first;

	Value invoke() const;
};


extern NativeFuncStorage nativeFuncs;
extern std::map<String, NativeFuncStorage> ObjectFuncs;
extern GlobalFuncStorage globalFuncs;