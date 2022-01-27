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
	Script* script;

	VarStorage variables;

	Variable* FindVar(const String& name) {
		if (variables.find(name) != variables.end())
			return &variables.find(name)->second;
		else if (parent == nullptr) {
			if (script->vars.find(name) != script->vars.end()) {
				return &script->vars.find(name)->second;
			}
			else if (globalVars.find(name) != globalVars.end()) {
				return &globalVars.find(name)->second;
			}
			return nullptr;
		}
		else return parent->FindVar(name);
	}

	Scope(Scope* p, Script* s) {
		parent = p;
		script = s;
	}
	~Scope()
	{
		for (const auto& p : childs) delete p;
	}
};
