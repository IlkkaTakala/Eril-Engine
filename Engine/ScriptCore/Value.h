#pragma once
#include "defines.h"


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
