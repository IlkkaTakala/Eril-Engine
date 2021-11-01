#pragma once
#include <Core.h>
#include "SceneComponent.h"

class Actor : public SceneComponent, public Tickable
{
public:
	Actor() {}
	virtual ~Actor() {};
	//void SetModel(const char* NewModel) { Model = NewModel; }
protected:
	virtual void Tick(float Delta) override;
	virtual void BeginPlay() override {};
};