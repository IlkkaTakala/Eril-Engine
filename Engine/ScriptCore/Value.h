#pragma once
#include "defines.h"
#include <variant>
#include "Error.h"

struct Value
{
	std::variant<void*, float, int, String, bool> value;

	Value() : value((void*)0) {}

	explicit Value(EVT type, const String& s) 
	{
		switch (type)
		{
		case EVT::Float:
			value = std::stof(s);
			break;
		case EVT::Int:
			value = std::stoi(s);
			break;
		case EVT::String:
			value = s;
			break;
		case EVT::Boolean:
			value = s == "true" ? true : false;
			break;
		default:
			value = (void*)0;
			break;
		}
	}

	Value& operator=(const Value& val) {
		value = val.value;
		return *this;
	}

	Value& operator=(Value&& val) noexcept {
		if (value.index() != 3) value = std::move(val.value);
		else value = val.value;
		return *this;
	} 

	~Value() {}

	Value(Value&& val) noexcept {
		if (value.index() != 3) value = std::move(val.value);
		else value = val.value;
	};

	Value(const Value& val) : value(val.value) {}

	Value(int in) : value(in) {}

	Value(float in) : value(in) {}

	Value(bool in) : value(in) {}

	Value(const String& in) : value(in) {}

	template <typename T>
	T GetValue() const
	{
		try {
			return std::get<T>(value);
		}
		catch (const std::bad_variant_access& ex) {
			std::cout << ex.what() << '\n';
			error("Invalid type conversion");
			return T();
		}
	}

	template<>
	bool GetValue() const
	{
		switch (value.index())
		{
		case 0:
			return false;
		case 1:
			return (bool)std::get<float>(value);
		case 2:
			return (bool)std::get<int>(value);
		case 3:
			return (std::get<String>(value) == "true" ? true : false);
		case 4:
			return std::get<bool>(value);
		default:
			return 0.f;
		}
	}

	template<>
	float GetValue() const 
	{
		switch (value.index())
		{
		case 0:
			return 0.f;
		case 1:
			return std::get<float>(value);
		case 2:
			return (float)std::get<int>(value);
		case 3:
			return std::stof(std::get<String>(value));
		default:
			return 0.f;
		}
	}

	template<>
	String GetValue() const
	{
		switch (value.index())
		{
		case 0:
			return "";
		case 1:
			return std::to_string(std::get<float>(value));
		case 2:
			return std::to_string(std::get<int>(value));
		case 3:
			return std::get<String>(value);
		case 4:
			return std::get<bool>(value) ? "true" : "false";
		default:
			return "";
		}
	}

	template<>
	int GetValue() const
	{
		switch (value.index())
		{
		case 0:
			return 0;
		case 1:
			return (int)std::get<float>(value);
		case 2:
			return std::get<int>(value);
		case 3:
			return std::stoi(std::get<String>(value));
		default:
			return 0;
		}
	}

	EVT type() const
	{
		switch (value.index())
		{
		case 0:
			return EVT::Null;
		case 1:
			return EVT::Float;
		case 2:
			return EVT::Int;
		case 3:
			return EVT::String;
		case 4:
			return EVT::Boolean;
		default:
			return EVT::Unknown;
		}
	}

	operator int() const
	{
		return GetValue<int>();
	}

	operator float() const
	{
		return GetValue<float>();
	}

	operator String() const
	{
		return GetValue<String>();
	}

	friend Value operator+(const Value& lhs, const Value& rhs) {
		switch (lhs.type())
		{
		case EVT::String:
		{
			return (String)lhs + (String)rhs;
		} break;

		case EVT::Float:
		{
			return lhs.GetValue<float>() + rhs.GetValue<float>();
		} break;

		case EVT::Int:
		{
			switch (rhs.type()) {
			case EVT::Int:
				return lhs.GetValue<int>() + rhs.GetValue<int>();
			case EVT::Float:
				return lhs.GetValue<float>() + rhs.GetValue<float>();
			default:
				return lhs;
				break;
			}
		} break;

		case EVT::Null:
		{
			return rhs + lhs;
		} break;
		default:
			return lhs;
		}
	}

	friend Value operator-(const Value& lhs, const Value& rhs) {
		switch (lhs.type())
		{
		case EVT::String:
		{
			String rval(rhs);
			if (rval.empty()) return lhs;
			size_t pos = std::string::npos;
			String res = lhs;
			while ((pos = res.find(rval)) != std::string::npos)
			{
				res.erase(pos, rval.length());
			}
			return Value(EVT::String, res);
		} break;

		case EVT::Float:
		{
			return lhs.GetValue<float>() - rhs.GetValue<float>();
		} break;

		case EVT::Int:
		{
			switch (rhs.type()) {
			case EVT::Int:
				return lhs.GetValue<int>() - rhs.GetValue<int>();
			case EVT::Float:
				return lhs.GetValue<float>() - rhs.GetValue<float>();
			default:
				return lhs;
				break;
			}
		} break;

		case EVT::Null:
		{
			switch (rhs.type())
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

	friend Value operator*(const Value& lhs, const Value& rhs) {
		switch (lhs.type())
		{
		case EVT::String:
		{
			return lhs;
		} break;

		case EVT::Float:
		{
			return lhs.GetValue<float>() * rhs.GetValue<float>();
		} break;

		case EVT::Int:
		{
			switch (rhs.type()) {
			case EVT::Int:
				return lhs.GetValue<int>() * rhs.GetValue<int>();
			case EVT::Float:
				return lhs.GetValue<float>() * rhs.GetValue<float>();
			default:
				return lhs;
				break;
			}
		} break;

		case EVT::Null:
		{
			return rhs;
		} break;
		default:
			return lhs;
		}
	}

	friend Value operator/(const Value& lhs, const Value& rhs) {
		switch (lhs.type())
		{
		case EVT::String:
		{
			return lhs;
		} break;

		case EVT::Float:
		{
			return lhs.GetValue<float>() / rhs.GetValue<float>();
		} break;

		case EVT::Int:
		{
			switch (rhs.type()) {
			case EVT::Int:
				return lhs.GetValue<int>() / rhs.GetValue<int>();
			case EVT::Float:
				return lhs.GetValue<float>() / rhs.GetValue<float>();
			default:
				return lhs;
				break;
			}
		} break;

		case EVT::Null:
		{
			return rhs;
		} break;
		default:
			return lhs;
		}
	}

	friend Value operator==(const Value& lhs, const Value& rhs) {
		switch (lhs.type())
		{
		case EVT::String:
		{
			return lhs.GetValue<String>() == rhs.GetValue<String>();
		} break;

		case EVT::Float:
		{
			float one = lhs.GetValue<float>();
			float two = rhs.GetValue<float>();
			return one == two || (one <= two + 0.0001 && one >= two - 0.0001);
		} break;

		case EVT::Int:
		{
			switch (rhs.type()) {
			case EVT::Int:
				return lhs.GetValue<int>() == rhs.GetValue<int>();
			case EVT::Float: {
				float one = lhs.GetValue<float>();
				float two = rhs.GetValue<float>();
				return one == two || (one <= two + 0.0001 && one >= two - 0.0001);
			}
			default:
				return false;
				break;
			}
		} break;
		
		case EVT::Boolean: 
		{
			return lhs.GetValue<bool>() == rhs.GetValue<bool>();
		} break;

		case EVT::Null:
		{
			return false;
		} break;
		default:
			return false;
		}
	}
};
