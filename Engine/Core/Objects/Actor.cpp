#include "Actor.h"
#include "Objects/InputComponent.h"

void Actor::LoadWithParameters(const String& args)
{
	InputComp = SpawnObject<InputComponent>();
	RegisterInputs(InputComp);
}

void Actor::Tick(float)
{
	//AddRotation(Vector(0, 0, 1.0));
}
