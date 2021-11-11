/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021
*/

#include "ComponentManager.h"




ComponentManager::ComponentManager()
{
}


ComponentManager::~ComponentManager()
{
	
	//TODO: Clear
	for (int i = 0; i < IndexUsage.size(); i++)
	{
		delete ComponentsPerType.at(i);
	}

	/*
	for (int i = 0; i < IndexUsage.size(); i++)
	{
		if (IndexUsage.at(i))
		{
			delete Components.at(i);
			Components.at(i) = nullptr;
			IndexUsage.at(i) = false;
		}
	}
	*/
}

int ComponentManager::AddComponent(Component* component, String typeName)
{
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
		std::vector<Component*>* newComponentArray = new std::vector<Component*>;
		ComponentsPerType.push_back(newComponentArray);
		std::vector<bool> b;
		IndexUsage.push_back(b);
	}
	int componentType = component->Type;
	int usedIndex = -1;
	std::vector<bool>& indexUsageVector = IndexUsage.at(componentType);


	for (int i = 0; i < indexUsageVector.size(); i++)
	{
		if (!indexUsageVector[i])
		{
			usedIndex = i;
			break;
		}
	}
	if (usedIndex == -1)
	{
		usedIndex = indexUsageVector.size();
		component->ID = usedIndex;
		ComponentsPerType.at(componentType)->push_back(component);
		IndexUsage.at(componentType).push_back(true);
	}
	else
	{
		component->ID = usedIndex;
		ComponentsPerType.at(componentType)->at(usedIndex) = component;
		IndexUsage.at(componentType).at(usedIndex) = true;
	}
	//ComponentsPerType.at(componentType)->at(usedIndex).Init();
	return usedIndex;
}

bool ComponentManager::RemoveComponent(int id, int type)
{
	if (type < IndexUsage.size())
	{
		if (id < IndexUsage.at(type).size())
		{
			delete ComponentsPerType.at(type)->at(id);
			IndexUsage.at(type).at(id) = false;
			ComponentsPerType.at(type)->at(id) = nullptr;		
			return true;
		}
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

Component* ComponentManager::GetComponent(int id, int type)
{
	if (type < IndexUsage.size())
	{
		if (id < IndexUsage.at(type).size())
		{
			return ComponentsPerType.at(type)->at(id);
		}
	}
	return nullptr;
}
