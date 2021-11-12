#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021, Last Edited 12.11.2021
*/

#include <vector>
#include <map>
#include "Core.h"
#include "ComponentTypeStorage.h"

class ComponentManager
{
public:
	ComponentManager();
	~ComponentManager();

	template <typename T>
	int AddComponent(T &component, String typeName)
	{
		int typeID = GetTypeIdByName(typeName);
		for (int i = 0; i < componentStorages.size(); i++)
		{
			if (componentStorages.at(i)->GetType() == typeID)
			{
				//component.Type = componentStorages.at(i)->GetType();
				ComponentTypeStorage<T>* s = static_cast<ComponentTypeStorage<T>*>(componentStorages.at(i));
				return s->AddComponent();
			}
		}
		return -1;
	}

	template <typename T>
	int AddComponent(T& component, int typeID)
	{
		for (int i = 0; i < componentStorages.size(); i++)
		{
			if (componentStorages.at(i)->GetType() == typeID)
			{
				//component.Type = componentStorages.at(i)->GetType();
				ComponentTypeStorage<T>* s = static_cast<ComponentTypeStorage<T>*>(componentStorages.at(i));
				return s->AddComponent();
			}
		}
		return -1;
	}

	template <typename T>
	Component* GetComponent(int id, String typeName, T type)
	{
		int typeID = GetTypeIdByName(typeName);
		for (int i = 0; i < componentStorages.size(); i++)
		{
			if (componentStorages.at(i)->GetType() == typeID)
			{
				ComponentTypeStorage<T>* s = static_cast<ComponentTypeStorage<T>*>(componentStorages.at(i));
				Component* c = s->GetComponent(id);
				return c;
			}
		}
		return nullptr;
	}

	template <typename T>
	Component* GetComponent(int id, int typeID, T type)
	{
		for (int i = 0; i < componentStorages.size(); i++)
		{
			if (componentStorages.at(i)->GetType() == typeID)
			{
				ComponentTypeStorage<T>* s = static_cast<ComponentTypeStorage<T>*>(componentStorages.at(i));
				Component* c = s->GetComponent(id);
				return c;
			}
		}
		return nullptr;
	}

	template <typename T>
	bool RemoveComponent(int id, String typeName, T type)
	{
		int typeID = GetTypeIdByName(typeName);
		for (int i = 0; i < componentStorages.size(); i++)
		{
			if (componentStorages.at(i)->GetType() == typeID)
			{
				ComponentTypeStorage<Component>* s = static_cast<ComponentTypeStorage<Component>*>(componentStorages.at(i));
				return s->RemoveComponent(id);
			}
		}
		return false;
	}

	template <typename T>
	bool RemoveComponent(int id, int typeID, T type)
	{
		for (int i = 0; i < componentStorages.size(); i++)
		{
			if (componentStorages.at(i)->GetType() == typeID)
			{
				ComponentTypeStorage<Component>* s = static_cast<ComponentTypeStorage<Component>*>(componentStorages.at(i));
				return s->RemoveComponent(id);
			}
		}
		return false;
	}

	template <typename T>
	void CreateComponentTypeStorage(T base, String typeName)
	{
		TypeNames.insert(std::pair<String, int>(typeName, typeCount));
		componentStorages.push_back(new ComponentTypeStorage<T>(typeCount));
		typeCount++;
	}

	int GetTypeIdByName(String typeName)
	{
		std::map<String, int>::iterator it = TypeNames.find(typeName);
		if (it != TypeNames.end())
		{
			return it->second;
		}
		return -1;
	}

private:
	std::vector<IComponentStorage*> componentStorages;
	std::map<String, int> TypeNames;
	int typeCount = 0;
};