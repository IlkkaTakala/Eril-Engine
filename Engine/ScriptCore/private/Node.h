#pragma once
#include "defines.h"
#include <array>
#include <functional>
#include <map>

#include <ScriptCore.h>
#include "Function.h"
#include "Error.h"

using namespace ScriptCore;

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
		setup = nullptr;
	}
	~Script()
	{
	}

	LocalFuncStorage functions;
	ScriptFunction* setup;
	VarStorage vars;
	Value execReturn;

	void evaluate()
	{
		_invoke_local(functions, "execute", execReturn);
	}
};

template <typename ...Args>
inline void _invoke(Value& value, const String& scope, const String& name, void* ptr, Args&... vals)
{
	if (nativeFuncs().find(scope) != nativeFuncs().end() && nativeFuncs()[scope].find(name) != nativeFuncs()[scope].end()) {
		auto c = dynamic_cast<Function<Args...>*>(nativeFuncs()[scope][name]);
		if (c) (*c)(value, ptr, vals...);
		else {
			warn(String("Function ") + name + " doesn't take arguments of type: " + ((typeName(vals.type()) + ", ") + ...));
			value = {};
		}
		return;
	}
	else if (globalFuncs.find(name) != globalFuncs.end()) {
		
	}
	error("Function " + scope + '.' + name + " not found");
	value = {};
}

template<>
inline void _invoke<>(Value& value, const String& scope, const String& name, void* ptr)
{
	if (nativeFuncs().find(scope) != nativeFuncs().end() && nativeFuncs()[scope].find(name) != nativeFuncs()[scope].end()) {
		auto c = dynamic_cast<Function<>*>(nativeFuncs()[scope][name]);
		if (c) (*c)(value, ptr);
		else value = {};
		return;
	}
	error("Function " + scope + '.' + name + " not found");
	value = {};
}

struct Node
{
	Node* child;
	Node* next;
	bool ref;

	virtual Node** GetChild(uint idx = 0) {
		return &child;
	}
	virtual void SetChild(uint idx, Node* n) { child = n; }
	virtual void SetValue(uint idx, const Value& v) {}

	Node() : child(nullptr), next(nullptr), ref(false) {}
	Node(const Node& node) : child(node.child), next(node.next) {}
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
	String scope;
	void* ptr;
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
			delete val;
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

	FuncNode(const String& s, const String& f, void* obj = nullptr)
	{
		ptr = obj;
		value = f;
		scope = s;
		params.fill(nullptr);
		eval_params.fill({});
	}

	virtual void evaluate(ScriptFunction* caller, Value& node) override
	{
		for (int i = 0; i < params.size(); i++) if (params[i]) params[i]->evaluate(caller, eval_params[i]);
		invoke_helper<c>(eval_params, std::make_index_sequence<c>(), node);
		for (int i = 0; i < params.size(); i++) 
			if (params[i]) {
				if (params[i]->ref) 
					params[i]->SetValue(i, eval_params[i]);
				eval_params[i].Clean();
			}
	}

private:
	template <std::size_t N, typename T, std::size_t... Indices>
	void invoke_helper(std::array<T, N>& v, std::index_sequence<Indices...>, Value& node) {
		_invoke(node, scope, value, ptr, std::get<Indices>(v)...);
	}
};

template<> inline void FuncNode<0>::evaluate(ScriptFunction* caller, Value& node)
{
	_invoke(node, scope, value, ptr);
};

typedef std::function<Node* (const String&, const String&, void*)> init_FuncNode;
static std::vector<init_FuncNode> FuncNodes{
	{[](const String& scope, const String& name, void* v) {return new FuncNode<0>(scope, name, v); }},
	{[](const String& scope, const String& name, void* v) {return new FuncNode<1>(scope, name, v); }},
	{[](const String& scope, const String& name, void* v) {return new FuncNode<2>(scope, name, v); }},
	{[](const String& scope, const String& name, void* v) {return new FuncNode<3>(scope, name, v); }},
	{[](const String& scope, const String& name, void* v) {return new FuncNode<4>(scope, name, v); }},
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
		params.resize(size);
	}

	virtual void evaluate(ScriptFunction* caller, Value& node) override
	{
		int i = 0;
		for (auto it = value->params.begin(); it != value->params.end(); it++) {
			if (i >= params.size()) break;
			if (params[i]) params[i]->evaluate(caller, *it->second.value);
			else *it->second.value = {};
			i++;
		}
		value->invoke(node);
		for (auto& p : value->params)
			p.second.value->Clean();
	}
};

struct VariableNode : public Node
{
	Variable* value;
	Node* index;
	bool isArray;

	VariableNode(Variable* ptr, bool array = false, Node* idx = nullptr, bool shouldDestroy = false) : value(ptr), isArray(array), index(idx) {}

	virtual ~VariableNode() { 
		delete index; 
	}

	virtual void SetValue(uint idx, const Value& v) {
		value->value->value = v.value;
	}

	virtual void evaluate(ScriptFunction* caller, Value& node) override
	{
		if (value->value->type() == EVT::Array && isArray) {
			Value* v = nullptr;
			if (index) {
				Value idxv;
				index->evaluate(caller, idxv);
				v = value->value->GetIndex((uint)(int64)idxv);
			}
			else {
				if (value->init) {
					v = value->value->PushIndex();
				}
				else {
					child->evaluate(caller, *value->value);
				}
			}
			
			if (v) {
				if (child && value->type == 0) child->evaluate(caller, *v);
				node = *v;
			}
		}
		else {
			if (child && value->type == 0) child->evaluate(caller, *(value->value));
			node = *value->value;
		}
	}
};

struct ConstantVariableNode : public Node
{
	Value* value;

	ConstantVariableNode(Value* ptr) : value(ptr){}

	virtual ~ConstantVariableNode() { }

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
		if (child) child->evaluate(caller, caller->returnValue);
		else caller->returnValue.Clean();
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
		body = nullptr;
	}
	virtual ~ForNode() { 
		delete begin;
		delete end;
		delete test;
		delete body;
	}

	Node* begin;
	Node* test;
	Node* end;
	Node* body;
	Value beginVal;

	virtual void evaluate(ScriptFunction* caller, Value& node) override
	{
		if (next && !body) {
			body = next;
			next = next->next;
			body->next = nullptr;
		}
		if (begin) begin->evaluate(caller, beginVal);
		if (test)
			for (test->evaluate(caller, beginVal); beginVal.toBool(); test->evaluate(caller, beginVal)) {
				body->evaluate(caller, node);
				end->evaluate(caller, beginVal);
			}
		else
			for (;;) {
				body->evaluate(caller, node);
				end->evaluate(caller, beginVal);
			}
	}
};

struct IfNode : public Node
{
	IfNode() {
		body = nullptr;
	}
	virtual ~IfNode() {
		delete body;
	}

	Node* body;

	virtual void evaluate(ScriptFunction* caller, Value& node) override
	{
		if (child) {
			Value testV;
			child->evaluate(caller, testV);
			if (testV.toBool()) 
				next->evaluate(caller, node);
			if (!body) body = next;
			if (next) {
				next = body->next;
				body->next = nullptr;
			}
		}
	}
};
