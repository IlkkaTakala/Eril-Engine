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

	operator float() const
	{
		return (float)atof(value.c_str());
	}

	operator String() const
	{
		return value;
	}

	friend Value operator+(const Value& lhs, const Value& rhs) {
		switch (lhs.type)
		{
		case EVT::String:
		{
			return Value(EVT::String, lhs.value + rhs.value);
		} break;

		case EVT::Float:
		{
			return (float)lhs + float(rhs);
		} break;

		case EVT::Null:
		{
			return rhs;
		} break;
		default:
			return lhs;
		}
	}

	friend Value operator-(const Value& lhs, const Value& rhs) {
		switch (lhs.type)
		{
		case EVT::String:
		{
			if (rhs.value.size() == 0) return lhs;
			size_t pos = std::string::npos;
			String res = lhs;
			while ((pos = res.find(rhs.value)) != std::string::npos)
			{
				res.erase(pos, rhs.value.length());
			}
			return Value(EVT::String, res);
		} break;

		case EVT::Float:
		{
			return Value(EVT::Float, std::to_string((float)lhs - float(rhs)));
		} break;

		case EVT::Null:
		{
			switch (rhs.type)
			{
			case EVT::Float:
				return -(float)rhs;
				break;
			default:
				return lhs;
				break;
			}
		} break;
		default:
			return lhs;
		}
	}
};
