#pragma once
#include <any>
#include "Function.h"

#define REGISTER_FUNCTION(NAME, SCOPE, ARG_C) static bool reg = AddFuncs<ARG_C>(#SCOPE, #NAME, &NAME);

unsigned __int32 CompileScript(const char* data);

void EvaluateScript(unsigned __int32 s);

void AddWrapped(const char* scope, const char* name, BaseFunction* function);

void CleanScript(unsigned __int32 script);

void GetError(const char* error, size_t size);

#include <tuple>

template <typename>
struct make_sig_from_tuple;

template <typename ...Args>
struct make_sig_from_tuple<std::tuple<Args...>> {
	using type = Function<Args...>;
};

template <typename ...Args>
using make_sig_from_tuple_t = typename make_sig_from_tuple<Args...>::type;

template <typename T, size_t N>
struct generate_sig_impl {
	using type = decltype(std::tuple_cat(std::declval<std::tuple<T>&>(), std::declval<typename generate_sig_impl<T, N - 1>::type&>()));
};

template <typename T>
struct generate_sig_impl<T, 0> {
	using type = std::tuple<>;
};

template <typename T, size_t N>
struct generate_sig {
	using type = make_sig_from_tuple_t<typename generate_sig_impl<T, N>::type>;
};

template <typename T, size_t N>
using generate_sig_t = typename generate_sig<T, N>::type;

template <int n, typename Func>
BaseFunction* make_wrap(Func f) {
	return new generate_sig_t<Value, n>(n, [f](void*, auto ...v) {
		return f(v...);
	});
}

template <int n, typename Ret, typename ...args>
bool AddFuncs(const char* scope, const char* name, Ret(*fun)(args...))
{
	AddWrapped(scope, name, make_wrap<n>(fun));
	return true;
}
