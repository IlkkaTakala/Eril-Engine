#pragma once
#include "defines.h"
#include <functional>
#include <any>
#include <map>
#include <iostream>

#include "Node.h"
#include "Error.h"

struct ScriptFunction;
struct Scope;

template <class R, class... ARGS>
struct function_ripper {
	static constexpr int n_args = sizeof...(ARGS);
};

template <class R, class... ARGS>
auto constexpr make_ripper(R(ARGS...)) {
	return function_ripper<R, ARGS...>();
}

template <typename...Args, typename Func>
BaseFunction* make_wrap(Func f) {
	int c = decltype(make_ripper(f))::n_args;
	return new Function<Args...>(c, [f](auto ...v) {
		return f(v...);
		});
}


static VarStorage globalVars;

struct Scope
{
	std::list<Scope*> childs;
	Scope* parent;

	VarStorage variables;

	Value* FindVar(const String& name) const {
		if (variables.find(name) != variables.end())
			return variables.find(name)->second;
		else if (parent == nullptr) {
			return nullptr;
		}
		else return parent->FindVar(name);
	}

	//Scope& operator[](uint idx) {
	//	if (idx == level) return *this;
	//	if (childs.size())
	//		childs.push_back(new Scope());
	//	return (*(*childs.rbegin()))[idx];
	//}

	Scope(Scope* p) {
		parent = p;
	}
	~Scope()
	{
		for (const auto& p : childs) delete p;
	}
};
