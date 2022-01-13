#pragma once
#include "Core.h"
#include <functional>
#include <map>
#include <queue>

// muuta komponentiksi objectille
// handle input inputcotrolin while loop
// inputcontrollissa kutsutaan input componentin handle inputtia
// baseobjectille register input
// InputComponent Priority Stack:

struct KeyActioni
{
	int key;
	int scancode;
	int action;
	int mods;
};

class InputComponent : public BaseObject, public Tickable
{
public:
	InputComponent();
	void BeginPlay(){ }
	void OnDestroyed() override;
	void Tick(float delta) override;

	void SetTextMode(bool mode) { isText = mode; }
	void HandleInputs(KeyActioni key, float delta);
	void HandleMouse(float x, float y, float delta);

	void ClearInputs() {
		KeyCallers.clear();
		KeyCallersHold.clear();
		MouseCallers.clear();
		TextCallers.clear();
	}

	template <class UserClass>
	void RegisterKeyContinuousInput(int Key, void (UserClass::* Callback)(float, bool), UserClass* Caller)
	{
		using std::placeholders::_1;
		using std::placeholders::_2;
		std::function<void(float, bool)> f = std::bind(Callback, Caller, _1, _2);
		KeyCallersHold.insert(std::pair<int, std::function<void(float, bool)>>(Key, f));
	}

	template <class UserClass>
	void RegisterKeyInput(int Key, void (UserClass::* Callback)(bool), UserClass* Caller)
	{
		using std::placeholders::_1;
		std::function<void(bool)> f = std::bind(Callback, Caller, _1);
		KeyCallers.insert(std::pair<int, std::function<void(bool)>>(Key, f));
	}

	template <class UserClass>
	void RegisterTextInput(void (UserClass::* Callback)(uint), UserClass* Caller)
	{
		using std::placeholders::_1;
		std::function<void(uint)> f = std::bind(Callback, Caller, _1);
		TextCallers.push_back(std::function<void(uint)>(f));
	}

	template <class UserClass>
	void RegisterMouseInput(int Key, void (UserClass::* Callback)(float, float), UserClass* Caller)
	{
		using std::placeholders::_1;
		using std::placeholders::_2;
		std::function<void(float, float)> f = std::bind(Callback, Caller, _1, _2);
		MouseCallers.insert(std::pair<int, std::function<void(float, float)>>(Key, f));
	}

protected:
	std::multimap<int, std::function<void(float, bool)>> KeyCallersHold;
	std::multimap<int, std::function<void(bool)>> KeyCallers;
	std::list<std::function<void(uint)>> TextCallers;
	std::multimap<int, std::function<void(float, float)>> MouseCallers;

	float MouseX;
	float MouseY;
	bool isText;

private:
	std::map<int, KeyActioni> Hold;
};