/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021, Last Edited by RabbitTortoise 24.11.2021
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

	//Console::Log("Added Entity To ID: " + std::to_string(usedIndex));
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
		if (IndexUsage.at(id) && !entity->GetProtected())
		{
			for (auto &c : entity->GetComponents())
			{
				WorldComponentManager->RemoveComponent<Component>(c.first, c.second);
			}

			delete Entities.at(id);
			Entities.at(id) = nullptr;
			IndexUsage.at(id) = false;
			entity = nullptr;
			return true;
		}
	}
	return false;
}

bool EntityManager::RemoveEntity(int entityID)
{
	if (entityID < IndexUsage.size())
	{
		if (IndexUsage.at(entityID) && !Entities.at(entityID)->GetProtected())
		{
			for (auto& c : GetEntity(entityID)->GetComponents())
			{
				WorldComponentManager->RemoveComponent<Component>(c.first, c.second);
			}

			delete Entities.at(entityID);
			Entities.at(entityID) = nullptr;
			IndexUsage.at(entityID) = false;
			return true;
		}
	}
	return false;
}


std::vector<int> EntityManager::QueryEntitiesByType(std::vector<int> allowedTypes)
{
	std::vector<int> query;
	bool found = false;
	for (int i = 0; i < static_cast<int>(Entities.size()); i++)
	{
		if (IndexUsage.at(i))
		{
			Entity* e = Entities.at(i);
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
	}
	return query;
}

int EntityManager::GetTypeIdByName(String typeName)
{
	return WorldComponentManager->GetTypeIdByName(typeName);
}

void EntityManager::ClearEntities()
{
	for (int i = 0; i < Entities.size(); i++)
	{
		if (IndexUsage[i])
		{
			RemoveEntity(Entities[i]);
		}
	}
}
