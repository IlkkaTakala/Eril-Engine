#include "pch.h"
#include "Function.h"


Value ArithPlus(Value lhs, Value rhs)
{
	return lhs + rhs;
}

Value ArithMinus(Value lhs, Value rhs)
{
	return lhs - rhs;
}

FuncStorage globalFuncs = {
	{"print", new Function<Value>(1, [](auto v) {
		std::cout << ">> " << v.GetValue<String>() << '\n';
		return Value();
	})},
	{"time", new Function<>(0, []() {
		float tim = (float)time(NULL);
		return Value(EVT::Float, std::to_string(tim));
	})},
	{"+", new Function<Value, Value>(2, ArithPlus)},
	{"-", new Function<Value, Value>(2, ArithMinus)},
};