#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021
*/

#include <Core.h>
#include <Objects/Actor.h>


#include "ECS/ECSWorldManager.h"
#include "Components/PositionComponent.h"
#include "Systems/MovementSystem.h"

class ECSTesting : public Actor
{
public:
	ECSTesting();

	virtual void Tick(float) override;
private:

	ECSWorldManager ECSWorld;
	int testID;
	int entityID;
};


