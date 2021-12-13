#pragma once
#include <functional>
#include <any>

typedef std::string String;

typedef unsigned char uint8;
typedef unsigned int uint;

struct Value;
struct BaseFunction;
struct ScriptFunction;
struct Scope;
struct Node;

typedef void(*FuncNoParam)();

typedef enum class EVariableType : uint8
{
	Null,
	Unknown,
	Float,
	String,
	Object,
	Array,
} EVT;

typedef enum class EConsiderType : uint8
{
	Null,
	Variable,
	Function,
	Operator,
} ECT;

enum class ECharType : uint8
{
	Digit,
	Char,
	Operand,
	Space,
	Other,
};

struct Value
{
	EVariableType type;
	String value;

	Value() : type(EVT::Null), value("") {}
	Value(EVT type, const String& s) : type(type), value(s) {}

	template <typename T>
	Value(T in) : value(in), type(EVT::Unknown) { }

	template<>
	Value(float in) { value = std::to_string(in); type = EVT::Float; }

	template<>
	Value(int in) { value = std::to_string(in); type = EVT::Float; }

	template <typename T>
	T GetValue() const
	{
		
	}

	template<>
	float GetValue() const
	{
		return (float)atof(value.c_str());
	}

	template<>
	String GetValue() const
	{
		return value;
	}

	template<>
	int GetValue() const
	{
		return (int)atof(value.c_str());
	}

	operator int() const
	{
		return (int)atof(value.c_str());
	}

	operator String() const 
	{
		return value;
	}
};

struct ScriptFunction
{
	ScriptFunction() {
		scope = nullptr;
		first = nullptr;
	}
	~ScriptFunction();

	Scope* scope;

	Node* first;

	Value invoke() const;
};

struct BaseFunction
{
	BaseFunction(int c) : param_count(c) {}
	virtual ~BaseFunction() {}
	static int GetParamCount(const String& name);
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