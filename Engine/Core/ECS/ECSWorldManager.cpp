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

	SetupEngineSystems();
}

ECSWorldManager::~ECSWorldManager()
{
	delete WorldSystemsManager;
	delete WorldEntityManager;
	delete WorldComponentManager;
}

void ECSWorldManager::Update(float deltaTime)
{
	WorldSystemsManager->UpdateSystems(deltaTime);
}

void ECSWorldManager::SetupEngineSystems()
{
	LightControllerSystem* lightController = new LightControllerSystem(*WorldEntityManager);
	int systemIndex = WorldSystemsManager->AddSystem(lightController);
	WorldSystemsManager->AddWantedComponentType(systemIndex, "LightComponent");
}
