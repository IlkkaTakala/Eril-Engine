/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021
*/

#include "ECSWorldManager.h"

ECSWorldManager::ECSWorldManager()
{
	WorldComponentManager = new ComponentManager();
	WorldEntityManager = new EntityManager(*WorldComponentManager);
	WorldSystemsManager = new SystemsManager(*WorldEntityManager, *WorldComponentManager);
}

ECSWorldManager::~ECSWorldManager()
{
	delete WorldSystemsManager;
	delete WorldEntityManager;
	delete WorldComponentManager;
}

void ECSWorldManager::Tick(float deltaTime)
{
	WorldSystemsManager->UpdateSystems(deltaTime);
}
