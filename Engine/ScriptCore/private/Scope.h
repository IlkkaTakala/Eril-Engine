#pragma once
#include "defines.h"
#include <map>

#include "Node.h"
#include "Error.h"

struct ScriptFunction;

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
		for (const auto& v : variables) {
			v.second.value->Clean();
		}
	}
};

struct ScopeNode : public Node
{
	ScopeNode() {
		scope = nullptr;
		parent = nullptr;
	}
	virtual ~ScopeNode() {}

	ScopeNode* parent;
	Scope* scope;

	virtual void evaluate(ScriptFunction* caller, Value& node) override
	{
		if (child) {
			Node* ptr = child;
			while (ptr) {
				ptr->evaluate(caller, node);
				ptr = ptr->next;
			}
		}
		if (scope)
			for (const auto& v : scope->variables) {
				v.second.value->Reset();
			}
	}
};