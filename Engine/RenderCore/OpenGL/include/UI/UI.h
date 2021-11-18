#pragma once
#include <Core.h>

class UIComponent;
class Player;

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

