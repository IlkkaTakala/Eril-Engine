#pragma once
#include "defines.h"
#include <array>
#include <functional>
#include <map>

#include "Function.h"
#include "Error.h"

inline Value _invoke_local(LocalFuncStorage& storage, const String& name) {
	if (storage.find(name) != storage.end()) {
		return storage.find(name)->second.invoke();
	}
	else
		error(("Invalid function call: " + name).c_str());
	return Value();
}


struct Script
{
	Script()
	{
	}
	~Script()
	{
	}

	LocalFuncStorage functions;
	VarStorage vars;

	void evaluate()
	{
		_invoke_local(functions, "execute");
	}
};

template <typename ...Args>
inline Value _invoke(const String& name, Args... vals)
{
	if (nativeFuncs.find(name) != nativeFuncs.end()) {
		auto c = dynamic_cast<Function<Args...>*>(nativeFuncs[name]);
		if (c) return (*c)(vals...);
		else return Value();
	}
	else if (globalFuncs.find(name) != globalFuncs.end()) {
		
	}
	else if (ObjectFuncs.find(name) != ObjectFuncs.end()) {
		
	}
	return Value();
}

template<>
inline Value _invoke<>(const String& name)
{
	if (nativeFuncs.find(name) != nativeFuncs.end()) {
		auto c = dynamic_cast<Function<>*>(nativeFuncs[name]);
		if (c) return (*c)();
		else return Value();
	}
	return Value();
}

struct Node
{
	Node* child;
	Node* next;

	virtual Node** GetChild(uint idx = 0) {
		return &child;
	}

	Node() : child(nullptr), next(nullptr) {}
	virtual ~Node()
	{
		delete child;
		delete next;
	}

	virtual Value evaluate(ScriptFunction* caller) = 0;
};

template <int c>
struct FuncNode : public Node
{
	String value;
	std::array<Node*, c> params;
	std::array<Value, c> eval_params;

	virtual Node** GetChild(uint idx) override {
		if (idx >= params.size()) return nullptr;
		return &params[idx];
	}

	virtual ~FuncNode() {
		for (const auto& child : params) {
			delete child;
		}
	}

	FuncNode(const String& s)
	{
		value = s;
		params.fill(nullptr);
		eval_params.fill(Value());
	}

	virtual Value evaluate(ScriptFunction* caller) override
	{
		for (int i = 0; i < params.size(); i++) eval_params[i] = (params[i] ? params[i]->evaluate(caller) : Value());
		return invoke_helper<c>(eval_params, std::make_index_sequence<c>());
	}

private:
	template <std::size_t N, typename T, std::size_t... Indices>
	Value invoke_helper(const std::array<T, N>& v, std::index_sequence<Indices...>) {
		return _invoke(value, std::get<Indices>(v)...);
	}
};

template<> inline Value FuncNode<0>::evaluate(ScriptFunction* caller)
{
	return _invoke(value);
};

typedef std::function<Node* (String)> init_FuncNode;
static std::vector<init_FuncNode> FuncNodes{
	{[](String name) {return new FuncNode<0>(name); }},
	{[](String name) {return new FuncNode<1>(name); }},
	{[](String name) {return new FuncNode<2>(name); }},
	{[](String name) {return new FuncNode<3>(name); }},
	{[](String name) {return new FuncNode<4>(name); }},
};

struct ScriptFuncNode : public Node
{
	ScriptFunction* value;
	std::vector<Node*> params;

	virtual Node** GetChild(uint idx) override {
		if (idx >= params.size()) return nullptr;
		return &params[idx];
	}

	virtual ~ScriptFuncNode() {
		for (const auto& child : params) {
			delete child;
		}
	}

	ScriptFuncNode(Script* s, const String& f, int size)
	{
		value = &s->functions[f];
		params.resize(size, nullptr);
	}

	virtual Value evaluate(ScriptFunction* caller) override
	{
		for (int i = 0; i < value->params.size() && i < params.size(); i++) {
			value->params[i] = (params[i] ? params[i]->evaluate(caller) : Value());
		}
		return value->invoke();
	}
};

struct ValueNode : public Node
{
	Value value;

	ValueNode(EVT type, const String& s);

	virtual ~ValueNode() {}

	virtual Value evaluate(ScriptFunction* caller) override
	{
		return value;
	}
};

struct VariableNode : public Node
{
	Value* value;

	VariableNode(Value* ptr);

	virtual ~VariableNode() {}

	virtual Value evaluate(ScriptFunction* caller) override
	{
		if (child) *value = child->evaluate(caller);
		return *value;
	}
};

struct ConstantVariableNode : public Node
{
	Value* value;

	ConstantVariableNode(Value* ptr) {
		value = ptr;
	}

	virtual ~ConstantVariableNode() {}

	virtual Value evaluate(ScriptFunction* caller) override
	{
		return *value;
	}
};

struct ControlNode : public Node
{
	ControlNode() {
	}

	virtual ~ControlNode() {}

	virtual Value evaluate(ScriptFunction* caller) override
	{
		caller->returnValue = child->evaluate(caller);
		caller->shouldReturn = true;
		return caller->returnValue;
	}
};