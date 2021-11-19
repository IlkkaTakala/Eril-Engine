/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021, Last Edited by RabbitTortoise 18.11.2021
*/

#include <ECS/EntityManager.h>


Entity* EntityManager::AddEntity()
{
	Entity* entity = new Entity();

	int usedIndex = -1;
	for (int i = 0; i < IndexUsage.size(); i++)
	{
		if (!IndexUsage[i])
		{
			usedIndex = i;
			break;
		}
	}
	if (usedIndex == -1)
	{
		usedIndex = static_cast<int>(IndexUsage.size());

		Entities.push_back(entity);
		IndexUsage.push_back(true);
	}
	else
	{
		Entities.at(usedIndex) = entity;
		IndexUsage.at(usedIndex) = true;
	}

	entity->SetEnabled(true);
	entity->SetID(usedIndex);

	return entity;
}

Entity* EntityManager::GetEntity(int entityIndex)
{
	if (entityIndex < IndexUsage.size())
	{
		if (IndexUsage.at(entityIndex))
		{
			return Entities.at(entityIndex);
		}
	}
	return nullptr;
}


bool EntityManager::RemoveEntity(Entity*& entity)
{
	int id = entity->GetID();
	for (int i = 0; i < IndexUsage.size(); i++)
	{
		if (IndexUsage.at(id))
		{
			delete Entities.at(id);
			Entities.at(id) = nullptr;
			IndexUsage.at(id) = false;
			entity = nullptr;
			return true;
		}
	}
	return false;
}

bool EntityManager::RemoveEntity(int entityIndex)
{
	if (entityIndex < IndexUsage.size())
	{
		if (IndexUsage.at(entityIndex))
		{
			delete Entities.at(entityIndex);
			Entities.at(entityIndex) = nullptr;
			IndexUsage.at(entityIndex) = false;
			return true;
		}
	}
	return false;
}


Component* EntityManager::CreateNewComponentToEntity(int entityIndex, String typeName)
{
	if (entityIndex < static_cast<int>(IndexUsage.size()))
	{
		if (IndexUsage.at(entityIndex))
		{
			Component* c = WorldComponentManager->CreateNewComponentOfType(typeName);
			int componentType = WorldComponentManager->GetTypeIdByName(typeName);
			Entities.at(entityIndex)->GetComponents().insert(std::pair<int, int>(c->GetID(), componentType));
			return c;
		}
	}
	return nullptr;
}

Component* EntityManager::CreateNewComponentToEntity(int entityIndex, int componentTypeID)
{
	if (entityIndex < static_cast<int>(IndexUsage.size()))
	{
		if (IndexUsage.at(entityIndex))
		{
			Component* c = WorldComponentManager->CreateNewComponentOfType(componentTypeID);
			Entities.at(entityIndex)->GetComponents().insert(std::pair<int, int>(c->GetID(), componentTypeID));
			return c;
		}
	}
	return nullptr;
}


Component* EntityManager::GetComponentFromEntity(int entityIndex, String componentType)
{
	int typeID = WorldComponentManager->GetTypeIdByName(componentType);

	Entity* entity = GetEntity(entityIndex);
	if (entity != nullptr)
	{

		for (auto& c : entity->GetComponents())
		{
			if (c.second == typeID)
			{
				Console::Log(std::to_string(Entities.at(entityIndex)->GetComponents().begin()->first) + "," + std::to_string(Entities.at(entityIndex)->GetComponents().begin()->first));
				Component* component = WorldComponentManager->GetComponent(entity->GetComponents().at(c.first), typeID);
				if (component != nullptr)
				{
					//Console::Log(std::to_string(component->GetID()));
					return component;
				}
				break;
			}
		}
	}
	return nullptr;
}

bool EntityManager::RemoveComponentFromEntity(int entityIndex, String componentType)
{
	return false;
}

std::vector<int> EntityManager::QueryEntitiesByType(std::vector<int> allowedTypes)
{
	std::vector<int> query;
	bool found = false;
	for (auto e : Entities)
	{
		if (e->GetEnabled())
		{
			for (auto t : e->GetComponents())
			{
				for (auto allowed : allowedTypes)
				{
					if (allowed == t.second)
					{
						query.push_back(e->GetID());
						break;
					}
				}
			}
		}
	}
	return query;
}


/// <summary>
/// Return typeName as an integer.
/// </summary>
/// <param name="typeName"></param>
/// <returns></returns>
int EntityManager::GetTypeIdByName(String typeName)
{
	return WorldComponentManager->GetTypeIdByName(typeName);
}
