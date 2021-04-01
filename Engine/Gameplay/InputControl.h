#pragma once
#include "Core.h"
#include <map>
#include <list>
#include <functional>

class Input
{
public:
	Input();
	~Input();

	template <class UserClass>
	static void RegisterKeyInput(int Key, void (UserClass::* Callback)(bool), UserClass* Caller) 
	{
		using std::placeholders::_1;
		std::function<void(bool)> f = std::bind(Callback, Caller, _1);
		KeyCallers.insert(std::pair<int, std::function<void(bool)>>(Key, f));
	}

	template <class UserClass>
	static void RegisterMouseInput(int Key, void (UserClass::* Callback)(float, float), UserClass* Caller)
	{
		using std::placeholders::_1;
		using std::placeholders::_2;
		std::function<void(float, float)> f = std::bind(Callback, Caller, _1, _2);
		MouseCallers.insert(std::pair<int, std::function<void(float, float)>>(Key, f));
	}

private:
	friend void InputCallback(int, int, int, int);
	static std::map<int, std::function<void(bool)>> KeyCallers;
	static std::map<int, std::function<void(float, float)>> MouseCallers;
	std::map<unsigned short, bool> KeyStates;
};