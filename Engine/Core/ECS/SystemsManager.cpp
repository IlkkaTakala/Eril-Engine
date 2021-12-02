/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021, Last Edited by RabbitTortoise 24.11.2021
*/

#include "SystemsManager.h"

int SystemsManager::AddSystem(System* system)
{
	system->bEnabled = true;
	Systems.push_back(system);
	return(static_cast<int>(Systems.size())-1);
}

bool SystemsManager::DisableSystem(int systemID)
{
	if (systemID < static_cast<int>(Systems.size()))
	{
		Systems.at(systemID)->bEnabled = false;
		return true;
	}
	return false;
}

bool SystemsManager::AddWantedComponentType(int systemID, String componentTypeName)
{
	if (systemID < static_cast<int>(Systems.size()))
	{
		int typeIndex = WorldComponentManager->GetTypeIdByName(componentTypeName);
		if (typeIndex != -1)
		{
			Systems.at(systemID)->TypesWanted.push_back(typeIndex);
			return true;
		}
	}
	return false;
}

void SystemsManager::UpdateSystems(float deltaTime)
{
	for (int i = 0; i < static_cast<int>(Systems.size()); i++)
	{
		if (Systems.at(i)->bEnabled)
		{
			IEntityQuerySystem* system = static_cast<IEntityQuerySystem*>(Systems.at(i));
			if (system != nullptr)
			{
				system->Update(deltaTime, WorldEntityManager->QueryEntitiesByType(Systems.at(i)->TypesWanted));
			}
		}
	}
}
