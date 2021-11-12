#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
12.11.2021
*/


#include <ECS/ComponentManager.h>
#include <ECS/EntityManager.h>
#include <ECS/SystemsManager.h>


namespace IECS
{
	void Init();
	void Destroy();

	ComponentManager* GetComponentManager();
	EntityManager* GetEntityManager();
	SystemsManager* GetSystemsManager();
}