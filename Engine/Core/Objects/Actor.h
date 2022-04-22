#pragma once
#include <Core.h>
#include "SceneComponent.h"

class Actor : public SceneComponent, public Tickable
{
public:
	Actor() {}
	virtual ~Actor() {};
	virtual void LoadWithParameters(const String& args);
	void DisableInput();
	void EnableInput();
	InputComponent* GetInputComponent();
	//void SetModel(const char* NewModel) { Model = NewModel; }
protected:
	virtual void Tick(float Delta) override;
	virtual void BeginPlay() override {};

private:
	Ref<InputComponent> InputComp;
};