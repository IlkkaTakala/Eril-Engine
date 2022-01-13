#pragma once
#include "defines.h"
#include <array>
#include <functional>
#include <map>

#include "Function.h"

template <typename ...Args>
inline Value _invoke(const String& name, Args... vals)
{
	if (globalFuncs.find(name) != globalFuncs.end()) {
		auto c = dynamic_cast<Function<Args...>*>(globalFuncs[name]);
		if (c) return (*c)(vals...);
		else return Value();
	}
	return Value();
}

template<>
inline Value _invoke<>(const String& name)
{
	if (globalFuncs.find(name) != globalFuncs.end()) {
		auto c = dynamic_cast<Function<>*>(globalFuncs[name]);
		if (c) return (*c)();
		else return Value();
	}
	return Value();
}

struct Node
{
	uint scope;
	Node* parent;
	Node* child;
	Node* next;

	virtual Node** GetChild(uint idx = 0) {
		return &child;
	}

	Node() : scope(0), parent(nullptr), child(nullptr), next(nullptr) {}
	virtual ~Node()
	{
		delete child;
		delete next;
	}

	virtual Value evaluate() = 0;
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

	virtual Value evaluate() override
	{
		for (int i = 0; i < params.size(); i++) eval_params[i] = (params[i] ? params[i]->evaluate() : Value());
		return invoke_helper<c>(eval_params, std::make_index_sequence<c>());
	}

private:
	template <std::size_t N, typename T, std::size_t... Indices>
	Value invoke_helper(const std::array<T, N>& v, std::index_sequence<Indices...>) {
		return _invoke(value, std::get<Indices>(v)...);
	}
};

template<> inline Value FuncNode<0>::evaluate()
{
	return _invoke(value);
};

typedef std::function<Node* (String)> init_FuncNode;
static std::map<int, init_FuncNode> FuncNodes{
	{0, [](String name) {return new FuncNode<0>(name); }},
	{1, [](String name) {return new FuncNode<1>(name); }},
	{2, [](String name) {return new FuncNode<2>(name); }},
	{3, [](String name) {return new FuncNode<3>(name); }},
	{4, [](String name) {return new FuncNode<4>(name); }},
};

struct ValueNode : public Node
{
	Value value;

	ValueNode(EVT type, const String& s);

	virtual ~ValueNode() {}

	virtual Value evaluate() override
	{
		return value;
	}
};