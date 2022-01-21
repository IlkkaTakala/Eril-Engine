#pragma once
#include "defines.h"
#include <array>
#include <functional>
#include <map>

#include "Function.h"
#include "Error.h"

inline void _invoke_local(LocalFuncStorage& storage, const String& name, Value& value) {
	if (storage.find(name) != storage.end()) {
		storage.find(name)->second.invoke(value);
		return;
	}
	else
		error(("Invalid function call: " + name).c_str());
	value = Value();
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
	Value execReturn;

	void evaluate()
	{
		_invoke_local(functions, "execute", execReturn);
	}
};

template <typename ...Args>
inline void _invoke(Value& value, const String& name, const Args&... vals)
{
	if (nativeFuncs.find(name) != nativeFuncs.end()) {
		auto c = dynamic_cast<Function<const Args&...>*>(nativeFuncs[name]);
		if (c) (*c)(value, vals...);
		else value = {};
		return;
	}
	else if (globalFuncs.find(name) != globalFuncs.end()) {
		
	}
	else if (ObjectFuncs.find(name) != ObjectFuncs.end()) {
		
	}
	value = {};
}

template<>
inline void _invoke<>(Value& value, const String& name)
{
	if (nativeFuncs.find(name) != nativeFuncs.end()) {
		auto c = dynamic_cast<Function<>*>(nativeFuncs[name]);
		if (c) (*c)(value);
		else value = {};
		return;
	}
	value = {};
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

	virtual void evaluate(ScriptFunction* caller, Value& node) = 0;
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
		eval_params.fill({});
	}

	virtual void evaluate(ScriptFunction* caller, Value& node) override
	{
		for (int i = 0; i < params.size(); i++) if (params[i]) params[i]->evaluate(caller, eval_params[i]); else eval_params[i] = {};
		invoke_helper<c>(eval_params, std::make_index_sequence<c>(), node);
	}

private:
	template <std::size_t N, typename T, std::size_t... Indices>
	void invoke_helper(const std::array<T, N>& v, std::index_sequence<Indices...>, Value& node) {
		_invoke(node, value, std::get<Indices>(v)...);
	}
};

template<> inline void FuncNode<0>::evaluate(ScriptFunction* caller, Value& node)
{
	_invoke(node, value);
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

	virtual void evaluate(ScriptFunction* caller, Value& node) override
	{
		for (int i = 0; i < value->params.size() && i < params.size(); i++) {
			if (params[i]) params[i]->evaluate(caller, value->params[i]);
			else value->params[i] = {};
		}
		value->invoke(node);
	}
};

struct ValueNode : public Node
{
	Value value;

	ValueNode(EVT type, const String& s);

	virtual ~ValueNode() {}

	virtual void evaluate(ScriptFunction* caller, Value& node) override
	{
		node = std::move(value);
	}
};

struct VariableNode : public Node
{
	Value* value;

	VariableNode(Value* ptr);

	virtual ~VariableNode() {}

	virtual void evaluate(ScriptFunction* caller, Value& node) override
	{
		if (child) child->evaluate(caller, *value);
		node = std::move(*value);
	}
};

struct ConstantVariableNode : public Node
{
	Value* value;

	ConstantVariableNode(Value* ptr) {
		value = ptr;
	}

	virtual ~ConstantVariableNode() {}

	virtual void evaluate(ScriptFunction* caller, Value& node) override
	{
		node = std::move(*value);
	}
};

struct ControlNode : public Node
{
	ControlNode() {
	}

	virtual ~ControlNode() {}

	virtual void evaluate(ScriptFunction* caller, Value& node) override
	{
		child->evaluate(caller, caller->returnValue);
		caller->shouldReturn = true;
		node = std::move(caller->returnValue);
	}
};