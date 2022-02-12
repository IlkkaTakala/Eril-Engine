#pragma once
#include <tuple>
#include <functional>
#include "Value.h"

#define REGISTER_FUNCTION(NAME, SCOPE, ARG_C) static bool reg_ ## SCOPE ## _ ## NAME = ScriptCore::AddFuncs<ARG_C>(#SCOPE, #NAME, &NAME);

namespace ScriptCore {

#ifdef _WIN32

	#ifdef SCRIPTCORE_EXPORTS
	#define SCRIPTCORE_API extern "C" __declspec(dllexport)
	#else
	#define SCRIPTCORE_API extern "C" __declspec(dllimport)
	#endif

	SCRIPTCORE_API unsigned long CompileScript(const char* data);
	SCRIPTCORE_API unsigned long CompileScriptWithName(const char* name, const char* data);

	SCRIPTCORE_API void EvaluateScript(unsigned long s);
	SCRIPTCORE_API void EvaluateScriptWithName(const char* name);
	SCRIPTCORE_API void EvaluateAll();

	SCRIPTCORE_API void CleanScript(unsigned long script);

	SCRIPTCORE_API void GetError(const char* error, size_t size);

#elif defined(unix) || defined(__unix__) || defined(__unix)

	extern "C" unsigned long CompileScript(const char* data);
	extern "C" unsigned long CompileScriptWithName(const char* name, const char* data);

	extern "C" void EvaluateScript(unsigned long s);
	extern "C" void EvaluateScriptWithName(const char* name);
	extern "C" void EvaluateAll();

	extern "C" void CleanScript(unsigned long script);

	extern "C" void GetError(const char* error, size_t size);

#endif


	//
	// Helper functions 
	//

	template <typename>
	struct make_sig_from_tuple;


	struct BaseFunction
	{
		BaseFunction(int c) : param_count(c) {}
		virtual ~BaseFunction() {}
		int param_count;
	};

	template<typename...Args>
	struct Function : public BaseFunction
	{
		typedef std::function<Value(void*, Args&...)> FuncParams;

		Function(int count, FuncParams func) : BaseFunction(count), call(func) {}
		Function() : BaseFunction(0), call(nullptr) {}

		FuncParams call;

		void operator()(Value& val, void* ptr, Args&... v) const {
			if (call) val = call(ptr, v...);
			else val = {};
		}

	};
	
#ifdef _WIN32
	SCRIPTCORE_API void AddWrapped(const char* scope, const char* name, BaseFunction* function);
#elif defined(unix) || defined(__unix__) || defined(__unix)
	extern "C" void AddWrapped(const char* scope, const char* name, BaseFunction * function);
#endif

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
		return new generate_sig_t<Value, n>(n, [f](void*, auto& ...v) {
			return f(v...);
		});
	}

	template <int n, typename Ret, typename ...args>
	bool AddFuncs(const char* scope, const char* name, Ret(*fun)(args...))
	{
		AddWrapped(scope, name, make_wrap<n>(fun));
		return true;
	}
}