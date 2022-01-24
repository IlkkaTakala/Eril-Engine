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
		auto c = dynamic_cast<Function<Args...>*>(nativeFuncs[name]);
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
	virtual void SetChild(uint idx, Node* n) { child = n; }
	virtual void SetValue(uint idx, const Value& v) {}

	Node() : child(nullptr), next(nullptr) {}
	virtual ~Node()
	{
		delete child;
		delete next;
	}

	virtual void evaluate(ScriptFunction* caller, Value& node) = 0;
};

struct ValueNode : public Node
{
	Value value;

	ValueNode(EVT type, const String& s) : value(Value(type, s)) {}
	ValueNode(const Value& v) : value(v) {}

	virtual ~ValueNode() {}

	virtual void evaluate(ScriptFunction* caller, Value& node) override
	{
		node = value;
	}
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

	virtual void SetChild(uint idx, Node* n) override {
		if (idx >= params.size()) return;
		if (ValueNode* val(dynamic_cast<ValueNode*>(n)); val) {
			params[idx] = nullptr;
			val->evaluate(nullptr, eval_params[idx]);
		}
		else {
			params[idx] = n;
		}
		return;
	}

	virtual void SetValue(uint idx, const Value& v) override {
		if (idx >= c) return;
		else eval_params[idx] = std::move(v);
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
		for (int i = 0; i < params.size(); i++) if (params[i]) params[i]->evaluate(caller, eval_params[i]);
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

	virtual void SetChild(uint idx, Node* n) override {
		if (idx >= params.size()) return;
		params[idx] = n;
		return;
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

struct VariableNode : public Node
{
	Variable* value;

	VariableNode(Variable* ptr) : value(ptr) {}

	virtual ~VariableNode() {}

	virtual void evaluate(ScriptFunction* caller, Value& node) override
	{
		if (child && value->type == 0) child->evaluate(caller, *(value->value));
		node = *value->value;
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

struct ForNode : public Node
{
	ForNode() {
		begin = nullptr;
		test = nullptr;
		end = nullptr;
	}
	virtual ~ForNode() {}

	Node* begin;
	Node* test;
	Node* end;
	Value beginVal;

	virtual void evaluate(ScriptFunction* caller, Value& node) override
	{
		if (begin) begin->evaluate(caller, beginVal);
		if (test)
		child->evaluate(caller, caller->returnValue);
		caller->shouldReturn = true;
		node = std::move(caller->returnValue);
	}
};

struct IfNode : public Node
{
	IfNode() {
		body = nullptr;
	}
	virtual ~IfNode() {}

	Node* body;

	virtual void evaluate(ScriptFunction* caller, Value& node) override
	{
		if (child) {
			Value testV;
			child->evaluate(caller, testV);
			if (testV.GetValue<bool>()) 
				next->evaluate(caller, node);
			if (!body) body = next;
			if (next) next = body->next;
		}
	}
};

struct ScopeNode : public Node
{
	ScopeNode() {
		parent = nullptr;
	}
	virtual ~ScopeNode() {}

	ScopeNode* parent;

	virtual void evaluate(ScriptFunction* caller, Value& node) override
	{
		if (child) {
			Node* ptr = child;
			while (ptr) {
				ptr->evaluate(caller, node);
				ptr = ptr->next;
			}
		}
	}
};