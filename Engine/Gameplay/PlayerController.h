#pragma once
#include "Core.h"
#include "IRender.h"

class UIHolder;

class Player : public BaseObject, public Tickable
{
public:
	Player();
	virtual ~Player() { delete PlayerCamera; }
	virtual void Tick(float Delta) override;
	virtual void BeginPlay() override {};

	template <class T>
	void SetUI() {
		auto n = new T();
		auto y = dynamic_cast<UIHolder*>(n);
		if (y != nullptr) {
			UI = y;
			GC::Pointers.emplace("Object_UI", y);
			y->Construct();
		}
	}

	Camera* GetCamera() const { return PlayerCamera; }

protected:
	Ref<UIHolder> UI;

private:
	Camera* PlayerCamera;
};

