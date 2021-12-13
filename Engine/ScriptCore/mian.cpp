#include <iostream>
#include <string>
#include <map>
#include <list>
#include <vector>
#include <cstdarg>
#include <array>
#include <sstream>

#include "ScriptCore.h"

struct Script;
struct Scope;

typedef std::map<String, BaseFunction*> FuncStorage;
typedef std::map<String, ScriptFunction> LocalFuncStorage;
typedef std::map<String, Value*> VarStorage;

String funner(String s, int i) {
	std::ostringstream repeated;
	std::fill_n(std::ostream_iterator<String>(repeated), i, s);
	return repeated.str();
}

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

static FuncStorage globalFuncs = {
	{"print", new Function<Value>(1, [](auto v) {
		std::cout << ">> " << v.GetValue<String>() << '\n';
		return Value();
	})},
	{"time", new Function<>(0, []() {
		return Value(EVT::Float, std::to_string((float)time(0)));
	})},
};

static VarStorage globalVars;

bool errorFlag;

std::map<uint, Script*> Scripts;

uint activeScript = 0;
uint activeScope = 0;

void error(const char* string = 0)
{
	std::cout << "Error while compiling: " << string << '\n';
	errorFlag = true;
}

inline ECharType TypeOfChar(const char* c)
{
	if (isdigit(*c)) {
		return ECharType::Digit;
	}
	else if (isalpha(*c)) {
		return ECharType::Char;
	}
	else if (isspace(*c)) {
		return ECharType::Digit;
	}
	else if (ispunct(*c)) {
		return ECharType::Operand;
	}
	else
		return ECharType::Other;
}

template <typename ...Args>
Value _invoke(const String& name, Args... vals)
{
	if (globalFuncs.find(name) != globalFuncs.end()) {
		auto c = dynamic_cast<Function<Args...>*>(globalFuncs[name]);
		if (c) return (*c)(vals...);
		else return Value();
	}
	return Value();
}

template<>
Value _invoke<>(const String& name)
{
	if (globalFuncs.find(name) != globalFuncs.end()) {
		auto c = dynamic_cast<Function<>*>(globalFuncs[name]);
		if (c) return (*c)();
		else return Value();
	}
	return Value();
}

Value _invoke_local(const LocalFuncStorage& storage, const String& name) {
	if (storage.find(name) != storage.end()) {
		return storage.find(name)->second.invoke();
	}
	else 
		error(("Invalid function call: " + name).c_str());
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
		for (int i = 0; i < params.size(); i++) eval_params[i] = params[i] ? params[i]->evaluate() : Value();
		return invoke_helper<c>(eval_params, std::make_index_sequence<c>());
	}

private:
	template <std::size_t N, typename T, std::size_t... Indices>
	Value invoke_helper(const std::array<T, N>& v, std::index_sequence<Indices...>) {
		return _invoke(value, std::get<Indices>(v)...);
	}
};

template<> Value FuncNode<0>::evaluate()
{
	return _invoke(value);
};

typedef std::function<Node* (String)> init_FuncNode;
static std::map<int, init_FuncNode> FuncNodes {
	{0, [](String name) {return new FuncNode<0>(name); }},
	{1, [](String name) {return new FuncNode<1>(name); }},
	{2, [](String name) {return new FuncNode<2>(name); }},
	{3, [](String name) {return new FuncNode<3>(name); }},
	{4, [](String name) {return new FuncNode<4>(name); }},
};

struct ValueNode : public Node
{
	Value value;

	ValueNode(EVT type, const String& s)
	{
		value = Value(type, s);
	}
	virtual ~ValueNode() {}

	virtual Value evaluate() override
	{
		return value;
	}
};

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


struct Scope
{
	int level;
	int childIdx;
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

	Scope& operator[](uint idx) {
		if (idx == level) return *this;
		if (childs.size())
			childs.push_back(new Scope());
		return (*(*childs.rbegin()))[idx];
	}

	Scope() {
		level = 0;
		parent = nullptr;
		childIdx = 0;
	}
	~Scope()
	{
		for (const auto& p : childs) delete p;
	}
};


bool parseLine(const char* data, uint off, uint& end, Node** nodePtr)
{	
	uint count = 0;
	const char* ptr = data + off;
	Node** prevScope = nullptr;
	Node** currentNode = nodePtr;

	String considerValue;
	EConsiderType type = ECT::Null;
	EVariableType valType = EVT::Null;

	bool argument = false;
	int param_count = 0;

	while (*ptr != '\0' && !errorFlag)
	{
		count++;
		if (*ptr == '\n') {
			count++;
			end = count + off;
			return true;
		}
		
		
		switch (TypeOfChar(ptr))
		{
		case ECharType::Digit:
		{
			switch (type)
			{
			case EConsiderType::Null:
			{
				type = ECT::Variable;
				valType = EVT::Float;
				considerValue = *ptr;
			} break;
			case EConsiderType::Variable:
			{
				considerValue += *ptr;
			} break;
			case EConsiderType::Function:
				considerValue += *ptr;
				break;
			case EConsiderType::Operator:
				// TODO End operator here!

				type = ECT::Variable;
				valType = EVT::Float;
				considerValue = *ptr;

				break;
			default:
				error("Unexpected error, found digit");
				break;
			}
		}
			break;
		case ECharType::Char:
		{
			switch (type)
			{
			case EConsiderType::Null:
				considerValue += *ptr;
				break;
			case EConsiderType::Variable:
				switch (valType)
				{
				case EVariableType::Null:
					break;
				case EVariableType::Float:
					error("Invalid number");
					break;
				case EVariableType::String:
					considerValue += *ptr;
					break;
				case EVariableType::Object:
					break;
				case EVariableType::Array:
					break;
				default:
					break;
				}
				break;
			case EConsiderType::Function:
				break;
			case EConsiderType::Operator:
				break;
			default:
				break;
			}
		}
			break;
		case ECharType::Operand:
		{
			switch (type)
			{
			case EConsiderType::Null:
				switch (*ptr)
				{
				case '(':
					*currentNode = FuncNodes[BaseFunction::GetParamCount(considerValue)](considerValue);
					prevScope = currentNode;
					currentNode = (*currentNode)->GetChild(0);
					considerValue = "";
					argument = true;
					break;
				case ')':
					argument = false;
					break;
				case ',':
					currentNode = (*prevScope)->GetChild(++param_count);
					break;
				case '"':
					type = ECT::Variable;
					valType = EVT::String;
					break;
				default:
					break;
				}
				break;
			case EConsiderType::Variable:
				switch (*ptr)
				{
				case '"':
					if (valType == EVT::String) {
						*currentNode = new ValueNode(valType, considerValue);
						valType = EVT::Null;
						type = ECT::Null;
					}
					else error("Invalid \" here");
					break;
				default:
					break;
				}
				break;
			case EConsiderType::Function:
				break;
			case EConsiderType::Operator:
				break;
			default:
				break;
			}
		}
			break;
		case ECharType::Space:
		{
			switch (type)
			{
			case EConsiderType::Null:
			{
				// TODO: variable name!
				
				considerValue = "";
			}
				break;
			case EConsiderType::Variable:
			{
				switch (valType)
				{
				case EVariableType::Null:
					break;
				case EVariableType::Float:
					*currentNode = new ValueNode(valType, considerValue);
					currentNode = (*currentNode)->GetChild();
					considerValue = "";
					type = ECT::Null;
					valType = EVT::Null;
					break;
				case EVariableType::String:
					considerValue += ' ';
					break;
				case EVariableType::Object:
					break;
				case EVariableType::Array:
					break;
				default:
					break;
				}
			} break;
			case EConsiderType::Function:
				// Thinking function arguments?
				break;
			case EConsiderType::Operator:
			{
				// TODO: End operator here!

				considerValue = "";
				type = ECT::Null;
			} break;
			default:
				error("Unexpected whitespace found");
				break;
			}
		}
			break;
		case ECharType::Other:
		{
			error("Weird character found");
		}
			break;
		default:
			break;
		}

		ptr++;
	}

	if (errorFlag) return false;
	switch (type)
	{
	case EConsiderType::Null:
	{

	}
		break;
	case EConsiderType::Variable:
	{
		*currentNode = new ValueNode(valType, considerValue);
	}
		break;
	case EConsiderType::Function:
	{

	}
		break;
	case EConsiderType::Operator:
	{

	}
		break;
	default:
		break;
	}

	end = count + off;
	return false;
}

void parseBlock()
{

}

void CleanScript(uint script)
{
	if (Scripts.find(script) != Scripts.end()) delete Scripts[script];
	Scripts.erase(script);
}

uint CompileScript(const String& data)
{
	errorFlag = false;
	activeScope = 0;
	uint off = 0;
	uint idx = Scripts.size() ? Scripts.rbegin()->first + 1 : 1;
	Scripts[idx] = new Script();
	activeScript = idx;
	Node** ptr = &(Scripts[idx]->functions["execute"].first);
	while (parseLine(data.c_str(), off, off, ptr) && !errorFlag);

	if (errorFlag) {
		CleanScript(idx);
		return 0;
	}

	return idx;
}

void EvaluateScript(uint s)
{
	if (Scripts.find(s) == Scripts.end()) return;
	Scripts[s]->evaluate();
}

int BaseFunction::GetParamCount(const String& name)
{
	return globalFuncs[name]->param_count;
}

Value ScriptFunction::invoke() const
{
	if (first) return first->evaluate();
	else return Value();
}

ScriptFunction::~ScriptFunction()
{
	delete scope;
	delete first;
}

void AddFuncs()
{
	globalFuncs["funner"] = make_wrap<Value, Value>(funner);
}

int main()
{
	AddFuncs();
	while (true) {
		std::cout << "Input your code: \n";
		String data;
		std::getline(std::cin, data);

		uint id = CompileScript(data);

		//std::cout << id;

		EvaluateScript(id);
		CleanScript(id);
	}
	return 0;
}