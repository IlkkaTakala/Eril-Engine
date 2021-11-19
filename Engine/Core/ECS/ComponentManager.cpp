/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
18.11.2021, Last Edited by RabbitTortoise 18.11.2021
*/

#include <ECS/ComponentManager.h>

int ComponentManager::GetTypeIdByName(String typeName)
{
	std::map<String, int>::iterator it = TypeNames.find(typeName);
	if (it != TypeNames.end())
	{
		return it->second;
	}
	return -1;
}

Component* ComponentManager::CreateNewComponentOfType(String typeName)
{
	int typeID = GetTypeIdByName(typeName);
	for (int i = 0; i < componentStorages.size(); i++)
	{
		if (componentStorages.at(i)->GetType() == typeID)
		{
			return componentStorages.at(i)->AddComponent();
		}
	}
	return nullptr;
}

Component* ComponentManager::CreateNewComponentOfType(int typeID)
{
	for (int i = 0; i < componentStorages.size(); i++)
	{
		if (componentStorages.at(i)->GetType() == typeID)
		{
			return componentStorages.at(i)->AddComponent();
		}
	}
	return nullptr;
}

Component* ComponentManager::GetComponent(int id, int typeID)
{
	Console::Log("ID + Type: " + std::to_string(id) + "," + std::to_string(id));
	for (int i = 0; i < componentStorages.size(); i++)
	{
		if (componentStorages.at(i)->GetType() == typeID)
		{
			Component* c = componentStorages.at(i)->GetComponent(id);
			Console::Log("ComponentManager::GetComponent: " + std::to_string(c->GetID()));
			return c;
		}
	}
	return nullptr;
}

bool ComponentManager::RemoveComponent(int id, String typeName)
{
	int typeID = GetTypeIdByName(typeName);
	for (int i = 0; i < componentStorages.size(); i++)
	{
		if (componentStorages.at(i)->GetType() == typeID)
		{
			return componentStorages.at(i)->RemoveComponent(id);
		}
	}
	return false;
}

bool ComponentManager::RemoveComponent(int id, int typeID)
{
	for (int i = 0; i < componentStorages.size(); i++)
	{
		if (componentStorages.at(i)->GetType() == typeID)
		{
			return componentStorages.at(i)->RemoveComponent(id);
		}
	}
	return false;
}