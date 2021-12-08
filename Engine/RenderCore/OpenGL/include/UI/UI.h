#pragma once
#include <Core.h>

class UIComponent;
class Player;

template <class T>
UIComponent* CreateUI()
{
	return new T();
}

class UI : public BaseObject
{
public:
	UI();

	virtual void OnDestroyed() override;

	virtual void PreConstruct() {}
	virtual void Construct() {}
	virtual void PostConstruct() {}

	static void AddToScreen(UI* component, Player* Owner, int screen = 0);
	static void RemoveAllUI();
	void RemoveFromScreen(int screen = 0);

	static UI* LoadFromFile(const String& file);

	template <typename T>
	static bool do_ui_register(String name) {
		UIList().emplace(name, &CreateUI<T>);
		return true;
	}

	typedef UIComponent* (*UISpawnFunction)(void);
	static std::map<String, UISpawnFunction>& UIList()
	{
		static std::map<String, UISpawnFunction> Type;
		return Type;
	}

protected:

	void AddComponent(UIComponent* com);

	template <class T>
	T* Create() {
		return new T();
	}

private:
	virtual void BeginPlay() final override {}

	friend class UISpace;

	Player* Owner;
	UIComponent* TopLevel;
};

#define REGISTER_UI(CLASSNAME) private: inline static bool Registered = UI::do_ui_register<CLASSNAME>(#CLASSNAME);