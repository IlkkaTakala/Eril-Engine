#pragma once
#include <string>

typedef std::string String;

typedef unsigned char uint8;
typedef unsigned int uint;
#ifdef _WIN32
typedef __int64 int64;
#elif defined(unix) || defined(__unix__) || defined(__unix)
typedef long long int64;
#endif

typedef enum class EVariableType : uint8
{
	Null,
	Unknown,
	Float,
	Int,
	String,
	Boolean,
	Object,
	Array
} EVT;


inline String typeName(EVT t) {
	switch (t)
	{
	case EVT::Null:
		return "null";
	case EVT::Unknown:
		return "unknown";
	case EVT::Float:
		return "float";
	case EVT::Int:
		return "int";
	case EVT::String:
		return "string";
	case EVT::Boolean:
		return "boolean";
	case EVT::Object:
		return "object";
	case EVT::Array:
		return "array";
	default:
		return "unknown";
	}
}

struct Node;
struct ScopeNode;
struct Scope;
struct Script;
struct ScriptFunction;

struct Context
{
	Context() {
		topLevel = nullptr;
		function = nullptr;
		currentNode = nullptr;
		loopNode = nullptr;
		scopeNode = nullptr;
		scope = nullptr;
		loopScope = nullptr;
		row = 0;
		ptr = nullptr;
		begin = nullptr;
		end = nullptr;
	}

	Script* topLevel;
	ScriptFunction* function;
	String considerValue;
	String considerScope;
	Node** currentNode;
	Node* loopNode;
	ScopeNode* scopeNode;
	Scope* scope;
	Scope* loopScope;
	int row;
	const char* ptr;
	const char* begin;
	const char* end;
};