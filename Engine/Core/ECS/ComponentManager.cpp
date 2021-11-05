/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021
*/

#include "ComponentManager.h"

ComponentManager::~ComponentManager()
{
	for (int i = 0; i < IndexUsage.size(); i++)
	{
		if (IndexUsage.at(i))
		{
			delete Components.at(i);
			Components.at(i) = nullptr;
			IndexUsage.at(i) = false;
		}
	}
}

int ComponentManager::AddComponent(Component* component, String typeName)
{

	int usedIndex = -1;
	for (int i = 0; i < IndexUsage.size(); i++)
	{
		if (IndexUsage[i] = i)
		{
			usedIndex = i;
			break;
		}
	}
	if (usedIndex == -1)
	{
		usedIndex = IndexUsage.size();

		Components.push_back(component);
		IndexUsage.push_back(true);
	}
	else
	{
		Components.at(usedIndex) = component;
		IndexUsage.at(usedIndex) = true;
	}

	//Give component index
	component->ID = usedIndex;

	//Check if that type of component is already in use, if not, add it to the list
	std::map<String, int>::iterator it = TypeNames.find(typeName);
	if (it != TypeNames.end())
	{
		component->Type = it->second;
	}
	else
	{
		component->Type = typeCount;
		TypeNames.insert(std::pair<String, int>(typeName, typeCount));
		typeCount++;
	}

	return usedIndex;
}

bool ComponentManager::RemoveComponent(int id)
{
	if (id < IndexUsage.size())
	{
		delete Components.at(id);
		Components.at(id) = nullptr;
		IndexUsage.at(id) = false;
		return true;
	}
	return false;
}

int ComponentManager::GetTypeByName(String typeName)
{

	std::map<String, int>::iterator it = TypeNames.find(typeName);
	if (it != TypeNames.end())
	{
		return it->second;
	}
	return -1;
}

Component* ComponentManager::GetComponent(int id)
{
	if (id < IndexUsage.size())
	{
		return Components.at(id);
	}
	return nullptr;
}
