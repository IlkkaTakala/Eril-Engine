#pragma once
#include "../private/defines.h"
#include <variant>
#include "../private/Error.h"
#include <list>
#include <vector>

struct Value
{
	typedef std::vector<Value> Array;
	typedef std::pair<int, Array>* ArrayPtr;
	std::variant<void*, float, int64, String, bool, ArrayPtr> value;
	static inline std::list<std::pair<int, Array>> arrays;
	
	Value() : value((void*)0) {}

	explicit Value(EVT type, const String& s) 
	{
		switch (type)
		{
		case EVT::Float:
			value = std::stof(s);
			break;
		case EVT::Int:
			value = std::stoll(s);
			break;
		case EVT::String:
			value = s;
			break;
		case EVT::Boolean:
			value = s == "true" ? true : false;
			break;
		case EVT::Array:
			value = &arrays.emplace_back(1, Array());
			break;
		default:
			value = (void*)0;
			break;
		}
	}

	void Clean() {
		if (value.index() == 5 && std::get<ArrayPtr>(value) != nullptr) {
			std::get<ArrayPtr>(value)->first--;
		}
		value = (void*)0;
	}

	static void CleanArrays() {
		arrays.remove_if([](auto& p) { return p.first < 1; });
	}

	void Reset() {
		switch (value.index())
		{
		case 1:
			value = 0.f;
			break;
		case 2:
			value = 0LL;
			break;
		case 3:
			value = "";
			break;
		case 4:
			value = false;
			break;
		case 5:
			if (std::get<ArrayPtr>(value) != nullptr) {
				std::get<ArrayPtr>(value)->second.clear();
			} break;

		default:
			value = (void*)0;
		}
	}

	Value& operator=(const Value& val) {
		if (this == &val)
			return *this;

		if (value.index() == 5) {
			std::get<ArrayPtr>(value)->first--;
		}
		value = val.value;
		if (value.index() == 5) {
			std::get<ArrayPtr>(value)->first++;
		}
		return *this;
	}

	Value& operator=(Value&& val) noexcept {
		if (this == &val)
			return *this;

		if (value.index() == 5) {
			std::get<ArrayPtr>(value)->first--;
		}
		if (value.index() != 3) value = std::move(val.value);
		else value = val.value;
		if (value.index() == 5) {
			std::get<ArrayPtr>(value)->first++;
		}
		return *this;
	} 

	~Value() {
		if (value.index() == 5) {
			std::get<ArrayPtr>(value)->first--;
		}
	}

	Value(Value&& val) noexcept {
		if (value.index() != 3) value = std::move(val.value);
		else value = val.value;
		if (value.index() == 5) std::get<ArrayPtr>(value)->first++;
	};

	Value(const Value& val) : value(val.value) {
		if (value.index() == 5) std::get<ArrayPtr>(value)->first++;
	}

	Value&& DeepCopy() {
		Value res;
		if (value.index() == 5) {
			res.value = &arrays.emplace_back(1, std::get<ArrayPtr>(value)->second);
		}
		else {
			res.value = value;
		}
		return std::forward<Value>(res);
	}

	Value(int64 in) : value(in) {}

	Value(int in) : value((int64)in) {}

	Value(float in) : value(in) {}

	Value(double in) : value((float)in) {}

	Value(bool in) : value(in) {}

	Value(const String& in) : value(in) {}

	bool toBool() const
	{
		switch (value.index())
		{
		case 0:
			return false;
		case 1:
			return (bool)std::get<float>(value);
		case 2:
			return (bool)std::get<int64>(value);
		case 3:
			return (std::get<String>(value) == "true" ? true : false);
		case 4:
			return std::get<bool>(value);
		default:
			return 0.f;
		}
	}

	operator bool() const
	{
		return toBool();
	}

	operator float() const 
	{
		switch (value.index())
		{
		case 0:
			return 0.f;
		case 1:
			return std::get<float>(value);
		case 2:
			return (float)std::get<int64>(value);
		case 3:
			return std::stof(std::get<String>(value));
		default:
			return 0.f;
		}
	}

	operator String() const
	{
		switch (value.index())
		{
		case 0:
			return "Undefined";
		case 1:
			return std::to_string(std::get<float>(value));
		case 2:
			return std::to_string(std::get<int64>(value));
		case 3:
			return std::get<String>(value);
		case 4:
			return std::get<bool>(value) ? "true" : "false";
		case 5: 
		{
			String res = "{ ";
			for (const auto& v : std::get<ArrayPtr>(value)->second)
				res += (String)v + " ";
			return res + "}";
		}
		default:
			return "Undefined";
		}
	}

	operator int() const
	{
		switch (value.index())
		{
		case 0:
			return 0;
		case 1:
			return (int)std::get<float>(value);
		case 2:
			return (int)std::get<int64>(value);
		case 3:
			return std::stoi(std::get<String>(value));
		case 4:
			return (int)std::get<bool>(value);
		default:
			return 0;
		}
	}

	operator int64() const
	{
		switch (value.index())
		{
		case 0:
			return 0;
		case 1:
			return (int64)std::get<float>(value);
		case 2:
			return std::get<int64>(value);
		case 3:
			return std::stoll(std::get<String>(value));
		case 4:
			return (int64)std::get<bool>(value);
		default:
			return 0;
		}
	}

	operator bool()
	{
		return toBool();
	}

	operator float()
	{
		switch (value.index())
		{
		case 0:
			return 0.f;
		case 1:
			return std::get<float>(value);
		case 2:
			return (float)std::get<int64>(value);
		case 3:
			return std::stof(std::get<String>(value));
		default:
			return 0.f;
		}
	}

	operator String()
	{
		switch (value.index())
		{
		case 0:
			return "Undefined";
		case 1:
			return std::to_string(std::get<float>(value));
		case 2:
			return std::to_string(std::get<int64>(value));
		case 3:
			return std::get<String>(value);
		case 4:
			return std::get<bool>(value) ? "true" : "false";
		case 5:
		{
			String res = "{ ";
			for (const auto& v : std::get<ArrayPtr>(value)->second)
				res += (String)v + " ";
			return res + "}";
		}
		default:
			return "Undefined";
		}
	}

	operator int()
	{
		switch (value.index())
		{
		case 0:
			return 0;
		case 1:
			return (int)std::get<float>(value);
		case 2:
			return (int)std::get<int64>(value);
		case 3:
			return std::stoi(std::get<String>(value));
		case 4:
			return (int)std::get<bool>(value);
		default:
			return 0;
		}
	}

	operator int64()
	{
		switch (value.index())
		{
		case 0:
			return 0;
		case 1:
			return (int64)std::get<float>(value);
		case 2:
			return std::get<int64>(value);
		case 3:
			return std::stoll(std::get<String>(value));
		case 4:
			return (int64)std::get<bool>(value);
		default:
			return 0;
		}
	}

	operator int*()
	{
		if (value.index() == 2) {
			return &(int&)std::get<int64>(value);
		}
		return nullptr;
	}

	operator int64*()
	{
		return &std::get<int64>(value);
	}

	operator String*()
	{
		if (value.index() == 3) {
			return &std::get<String>(value);
		}
		return nullptr;
	}

	operator float*()
	{
		if (value.index() == 1) {
			return &std::get<float>(value);
		}
		return nullptr;
	}

	operator bool*()
	{
		if (value.index() == 4) {
			return &std::get<bool>(value);
		}
		return nullptr;
	}

	operator ArrayPtr*()
	{
		if (value.index() == 5) {
			return &std::get<ArrayPtr>(value);
		}
		return nullptr;
	}

	operator Array*() const
	{
		switch (value.index())
		{
		case 0:
			return nullptr;
		case 1:
			return nullptr;
		case 2:
			return nullptr;
		case 3:
			return nullptr;
		case 4:
			return nullptr;
		case 5:
			return &std::get<ArrayPtr>(value)->second;
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
		case 5:
			return EVT::Array;
		default:
			return EVT::Unknown;
		}
	}

	Value* GetIndex(unsigned int idx) {
		if (value.index() == 5) {
			Array& a = std::get<ArrayPtr>(value)->second;
			if (a.size() > idx && idx >= 0) {
				return &a.at(idx);
			}
			else {
				error("Index out of bounds: " + std::to_string(idx) + " from size of " + std::to_string(a.size()));
				return nullptr;
			}
		}
		else return this;
	}

	int GetSize() {
		if (value.index() == 5) {
			Array& a = std::get<ArrayPtr>(value)->second;
			return (int)a.size();
		}
		else return 1;
	}

	Value* PushIndex() {
		if (value.index() == 5) {
			return &std::get<ArrayPtr>(value)->second.emplace_back();
		}
		else return this;
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
			return (float)lhs + (float)rhs;
		} break;

		case EVT::Int:
		{
			switch (rhs.type()) {
			case EVT::Int:
				return (int64)lhs + (int64)rhs;
			case EVT::Float:
				return (float)lhs + (float)rhs;
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
				return +(float)rhs;
				break;
			case EVT::Int:
				return +(int64)rhs;
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
			return (float)lhs - (float)rhs;
		} break;

		case EVT::Int:
		{
			switch (rhs.type()) {
			case EVT::Int:
				return (int64)lhs - (int64)rhs;
			case EVT::Float:
				return (float)lhs - (float)rhs;
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
			case EVT::Int:
				return -(int64)rhs;
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
			return (float)lhs * (float)rhs;
		} break;

		case EVT::Int:
		{
			switch (rhs.type()) {
			case EVT::Int:
				return (int64)lhs * (int64)rhs;
			case EVT::Float:
				return (float)lhs * (float)rhs;
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
			return (float)lhs / (float)rhs;
		} break;

		case EVT::Int:
		{
			switch (rhs.type()) {
			case EVT::Int:
				return (int64)lhs / (int64)rhs;
			case EVT::Float:
				return (float)lhs / (float)rhs;
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
			return (String)lhs == (String)rhs;
		} break;

		case EVT::Float:
		{
			float one = (float)lhs;
			float two = (float)rhs;
			return one == two || (one <= two + 0.0001 && one >= two - 0.0001);
		} break;

		case EVT::Int:
		{
			switch (rhs.type()) {
			case EVT::Int:
				return (int64)lhs == (int64)rhs;
			case EVT::Float: {
				float one = (float)lhs;
				float two = (float)rhs;
				return one == two || (one <= two + 0.0001 && one >= two - 0.0001);
			}
			default:
				return false;
				break;
			}
		} break;
		
		case EVT::Boolean: 
		{
			return lhs.toBool() == rhs.toBool();
		} break;

		case EVT::Null:
		{
			return false;
		} break;
		default:
			return false;
		}
	}

	friend Value operator!=(const Value& lhs, const Value& rhs) {
		return !(lhs == rhs);
	}

	friend Value operator<(const Value& lhs, const Value& rhs) {
		switch (lhs.type())
		{
		case EVT::String:
		{
			return (String)lhs < (String)rhs;
		} break;

		case EVT::Float:
		{
			float one = (float)lhs;
			float two = (float)rhs;
			return one < two;
		} break;

		case EVT::Int:
		{
			switch (rhs.type()) {
			case EVT::Int:
				return (int64)lhs < (int64)rhs;
			case EVT::Float: {
				float one = (float)lhs;
				float two = (float)rhs;
				return one < two;
			}
			default:
				return false;
				break;
			}
		} break;

		case EVT::Boolean:
		{
			return lhs.toBool() < rhs.toBool();
		} break;

		case EVT::Null:
		{
			return false;
		} break;
		default:
			return false;
		}
	}

	friend Value operator>(const Value& lhs, const Value& rhs) {
		switch (lhs.type())
		{
		case EVT::String:
		{
			return (String)lhs > (String)rhs;
		} break;

		case EVT::Float:
		{
			float one = (float)lhs;
			float two = (float)rhs;
			return one > two;
		} break;

		case EVT::Int:
		{
			switch (rhs.type()) {
			case EVT::Int:
				return (int64)lhs > (int64)rhs;
			case EVT::Float: {
				float one = (float)lhs;
				float two = (float)rhs;
				return one > two;
			}
			default:
				return false;
				break;
			}
		} break;

		case EVT::Boolean:
		{
			return lhs.toBool() > rhs.toBool();
		} break;

		case EVT::Null:
		{
			return false;
		} break;
		default:
			return false;
		}
	}

	friend Value operator<=(const Value& lhs, const Value& rhs) {
		return lhs < rhs || lhs == rhs;
	}

	friend Value operator>=(const Value& lhs, const Value& rhs) {
		return lhs > rhs || lhs == rhs;
	}

	friend Value operator-(const Value& rhs) {
		switch (rhs.type())
		{
		case EVT::String:
		{
			return rhs;
		} break;

		case EVT::Float:
		{
			return -(float)rhs;
		} break;

		case EVT::Int:
		{
			return -(int64)rhs;
		} break;

		case EVT::Null:
		{
			return rhs;
		} break;
		default:
			return rhs;
		}
	}
};