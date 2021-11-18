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

bool EntityManager::RemoveEntity(Entity& entity)
{
	for (int i = 0; i < IndexUsage.size(); i++)
	{
		if (IndexUsage.at(entity.GetID()))
		{
			int id = entity.GetID();
			delete Entities.at(id);
			Entities.at(id) = nullptr;
			IndexUsage.at(id) = false;
			return true;
		}
	}
	return false;
}

/*
bool EntityManager::AddComponentToEntity( int entityIndex, Component* component, String typeName)
{
	if (entityIndex < IndexUsage.size())
	{
		if (IndexUsage.at(entityIndex))
		{
			int componentID = WorldComponentManager.AddComponent(component, typeName);
			int componentType = WorldComponentManager.GetTypeByName(typeName);

			Entities.at(entityIndex)->GetComponents().push_back(componentID);
			Entities.at(entityIndex)->GetComponentTypes().push_back(componentType);
			return true;
		}
	}
	return false;
}


Component* EntityManager::GetComponentFromEntity(int entityIndex, String componentType)
{
	
	int typeID = WorldComponentManager.GetTypeByName(componentType);

	Entity* entity = GetEntity(entityIndex);
	if (entity != nullptr)
	{
		bool componentFound = false;
		for (auto c : entity->GetComponentTypes())
		{
			if (c == typeID)
			{
				componentFound = true;
				break;
			}
		}
		if (componentFound)
		{
			for (auto c : entity->GetComponents())
			{
				Component* component = WorldComponentManager.GetComponent(c, componentType);
				if (component != nullptr)
				{
					if (component->GetType() == typeID)
					{
						return component;
					}
				}
			}
		}
	}
	return nullptr;
	
}
*/

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
