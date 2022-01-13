#include "pch.h"
#include "Function.h"

FuncStorage globalFuncs = {
	{"print", new Function<Value>(1, [](auto v) {
		std::cout << ">> " << v.GetValue<String>() << '\n';
		return Value();
	})},
	{"time", new Function<>(0, []() {
		return Value(EVT::Float, std::to_string((float)time(0)));
	})},
};