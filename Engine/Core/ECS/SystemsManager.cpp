/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021, Last Edited by RabbitTortoise 24.11.2021
*/

#include "SystemsManager.h"

int SystemsManager::AddSystem(System* system, String systemName)
{
	system->bEnabled = true;
	Systems.push_back(system);
	system->SetID(static_cast<int>(Systems.size()) - 1);
	SystemNames.insert(std::pair<String, int>(systemName, system->GetID()));

	return(system->GetID());
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
			if (Systems.at(i)->GetType() == System::SystemType::IEntityQuerySystem)
			{
				IEntityQuerySystem* eqSys = static_cast<IEntityQuerySystem*>(Systems.at(i));
				eqSys->EntityQueryUpdate(deltaTime, WorldEntityManager->QueryEntitiesByType(eqSys->TypesWanted));
				continue;
			}
			
			if (Systems.at(i)->GetType() == System::SystemType::IComponentArrayQuerySystem)
			{
				Systems.at(i)->Update(deltaTime);

				continue;
			}	
		}
	}
}

System* SystemsManager::GetSystemByName(String systemName)
{
	int id = -1;
	id = SystemNames.find(systemName)->second;
	for (System* s : Systems)
	{
		if (s->GetID() == id)
		{
			return s;
		}
	}
	return nullptr;
}

void SystemsManager::ClearComponentArraySystems()
{
	for (int i = 0; i < static_cast<int>(Systems.size()); i++)
	{
		if (Systems.at(i)->GetType() == System::SystemType::IComponentArrayQuerySystem)
		{
			
			IComponentArrayQuerySystem<Component>* caSys = static_cast<IComponentArrayQuerySystem<Component>*>(Systems.at(i));
			caSys->ResetSystem();
		}	
	}

}

