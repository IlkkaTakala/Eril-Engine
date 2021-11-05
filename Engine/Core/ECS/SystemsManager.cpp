/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021
*/

#include "SystemsManager.h"

int SystemsManager::AddSystem(System* system)
{
	system->bEnabled = true;
	Systems.push_back(system);
	return(Systems.size()-1);
}

bool SystemsManager::DisableSystem(int systemIndex)
{
	if (systemIndex < Systems.size())
	{
		Systems.at(systemIndex)->bEnabled = false;
		return true;
	}
	return false;
}

bool SystemsManager::AddWantedComponentType(int systemIndex, String componentType)
{
	if (systemIndex < Systems.size())
	{
		int typeIndex = WorldComponentManager.GetTypeByName(componentType);
		if (typeIndex != -1)
		{
			Systems.at(systemIndex)->TypesWanted.push_back(typeIndex);
			return true;
		}
	}
	return false;
}

void SystemsManager::UpdateSystems(float deltaTime)
{
	for (int i = 0; i < Systems.size(); i++)
	{
		if (Systems.at(i)->bEnabled)
		{
			Systems.at(i)->Update(deltaTime, WorldEntityManager.QueryEntitiesByType(Systems.at(i)->TypesWanted));
		}
	}
}
