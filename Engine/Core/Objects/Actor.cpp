#include "Actor.h"
#include "Objects/InputComponent.h"

void Actor::LoadWithParameters(const String& args)
{
	InputComp = SpawnObject<InputComponent>();
	RegisterInputs(InputComp);
	SceneComponent::LoadWithParameters(args);
}

void Actor::DisableInput()
{
	InputComp->SetInputDisabled(true);
}

void Actor::EnableInput()
{
	InputComp->SetInputDisabled(false);
}

InputComponent* Actor::GetInputComponent()
{
	return InputComp;
}

void Actor::Tick(float)
{
	//AddRotation(Vector(0, 0, 1.0));
}


