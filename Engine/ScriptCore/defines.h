#pragma once
#include <string>

typedef std::string String;

typedef unsigned char uint8;
typedef unsigned int uint;

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