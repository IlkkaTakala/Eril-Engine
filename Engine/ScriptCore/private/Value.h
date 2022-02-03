#pragma once
#include "defines.h"
#include <variant>
#include "Error.h"
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

		value = val.value;
		if (value.index() == 5) std::get<ArrayPtr>(value)->first++;
		return *this;
	}

	Value& operator=(Value&& val) noexcept {
		if (this == &val)
			return *this;

		if (value.index() != 3) value = std::move(val.value);
		else value = val.value;
		if (value.index() == 5) {
			std::get<ArrayPtr>(value)->first++;
		}
		return *this;
	} 

	Value& operator=(void) {
		if (this == &val)
			return *this;

		value = val.value;
		if (value.index() == 5) std::get<ArrayPtr>(value)->first++;
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
			return (bool)std::get<int64>(value);
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
			return (float)std::get<int64>(value);
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

	template<>
	int64 GetValue() const
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
		case 5:
			return EVT::Array;
		default:
			return EVT::Unknown;
		}
	}

	operator int64() const
	{
		return GetValue<int64>();
	}

	operator int() const
	{
		return (int)GetValue<int64>();
	}

	operator float() const
	{
		return GetValue<float>();
	}

	operator String() const
	{
		return GetValue<String>();
	}

	operator bool() const
	{
		return GetValue<bool>();
	}

	Value* GetIndex(int idx) {
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
			return lhs.GetValue<float>() + rhs.GetValue<float>();
		} break;

		case EVT::Int:
		{
			switch (rhs.type()) {
			case EVT::Int:
				return lhs.GetValue<int64>() + rhs.GetValue<int64>();
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
				return lhs.GetValue<int64>() - rhs.GetValue<int64>();
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
				return lhs.GetValue<int64>() * rhs.GetValue<int64>();
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
				return lhs.GetValue<int64>() / rhs.GetValue<int64>();
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
				return lhs.GetValue<int64>() == rhs.GetValue<int64>();
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

	friend Value operator!=(const Value& lhs, const Value& rhs) {
		return !(lhs == rhs);
	}

	friend Value operator<(const Value& lhs, const Value& rhs) {
		switch (lhs.type())
		{
		case EVT::String:
		{
			return lhs.GetValue<String>() < rhs.GetValue<String>();
		} break;

		case EVT::Float:
		{
			float one = lhs.GetValue<float>();
			float two = rhs.GetValue<float>();
			return one < two;
		} break;

		case EVT::Int:
		{
			switch (rhs.type()) {
			case EVT::Int:
				return lhs.GetValue<int64>() < rhs.GetValue<int64>();
			case EVT::Float: {
				float one = lhs.GetValue<float>();
				float two = rhs.GetValue<float>();
				return one < two;
			}
			default:
				return false;
				break;
			}
		} break;

		case EVT::Boolean:
		{
			return lhs.GetValue<bool>() < rhs.GetValue<bool>();
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
			return lhs.GetValue<String>() > rhs.GetValue<String>();
		} break;

		case EVT::Float:
		{
			float one = lhs.GetValue<float>();
			float two = rhs.GetValue<float>();
			return one > two;
		} break;

		case EVT::Int:
		{
			switch (rhs.type()) {
			case EVT::Int:
				return lhs.GetValue<int64>() > rhs.GetValue<int64>();
			case EVT::Float: {
				float one = lhs.GetValue<float>();
				float two = rhs.GetValue<float>();
				return one > two;
			}
			default:
				return false;
				break;
			}
		} break;

		case EVT::Boolean:
		{
			return lhs.GetValue<bool>() > rhs.GetValue<bool>();
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
};