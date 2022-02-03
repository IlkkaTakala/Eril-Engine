#pragma once

#ifdef SCRIPTCORE_EXPORTS
#define SCRIPTCORE_API __declspec(dllexport)
#else
#define SCRIPTCORE_API __declspec(dllimport)
#endif

#define REGISTER_FUNCTION(NAME, SCOPE, ARG_C) static bool reg_ ## SCOPE ## _ ## NAME = AddFuncs<ARG_C>(#SCOPE, #NAME, &NAME);

extern "C" SCRIPTCORE_API unsigned __int32 CompileScript(const char* data);

extern "C" SCRIPTCORE_API void EvaluateScript(unsigned __int32 s);

extern "C" SCRIPTCORE_API void CleanScript(unsigned __int32 script);

extern "C" SCRIPTCORE_API void GetError(const char* error, size_t size);

//
// Helper functions 
//

#include <tuple>
#include <functional>

template <typename>
struct make_sig_from_tuple;


struct BaseFunction
{
	BaseFunction(int c) : param_count(c) {}
	virtual ~BaseFunction() {}
	int param_count;
};

template<typename R = void, typename...Args>
struct Function : public BaseFunction
{
	typedef std::function<R(void*, const Args&...)> FuncParams;

	Function(int count, FuncParams func) : BaseFunction(count), call(func) {}
	Function() : BaseFunction(0), call(nullptr) {}

	FuncParams call;

	R operator()(void* ptr, const Args&... v) const {
		if (call) return std::move(call(ptr, v...));
		else return R();
	}

};

void AddWrapped(const char* scope, const char* name, BaseFunction* function);

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
