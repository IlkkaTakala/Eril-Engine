#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021
*/

#include <vector>
#include <map>
#include "Core.h"
#include "Component.h"
#include "ComponentTypeStorage.h"

class BaseFactory{
public:
	virtual void* create() const = 0;
};

template <typename T>
class Factory : public BaseFactory {
public:
	T* create() const {
		return new T();
	}
};

class ComponentManager
{
	friend class ECSWorldManager;
	friend class EntityManager;

public:
	ComponentManager();
	~ComponentManager();

	template <typename T>
	int AddComponent(T &component, String typeName)
	{
		int typeID = GetTypeByName(typeName);
		for (int i = 0; i < componentStorages.size(); i++)
		{
			if (componentStorages.at(i)->GetType() == typeID)
			{
				//component.Type = componentStorages.at(i)->GetType();
				ComponentTypeStorage<T>* s = static_cast<ComponentTypeStorage<T>*>(componentStorages.at(i));
				s->AddComponent();
				return s->GetComponentCount()-1;
			}
		}
		return -1;
	}

	template <typename T>
	Component* GetComponent(int id, String typeName, T type)
	{
		int typeID = GetTypeByName(typeName);
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

	bool RemoveComponent(int id, String typeName)
	{
		int typeID = GetTypeByName(typeName);
		for (int i = 0; i < componentStorages.size(); i++)
		{
			if (componentStorages.at(i)->GetType() == typeID)
			{
				//ComponentTypeStorage<Component>* s = static_cast<ComponentTypeStorage<Component>*>(componentStorages.at(i));
				//return s->RemoveComponent(id);
			}
		}
		return false;
	}

	int GetTypeByName(String typeName)
	{
		std::map<String, int>::iterator it = TypeNames.find(typeName);
		if (it != TypeNames.end())
		{
			return it->second;
		}
	return -1;
	}

	template <typename T>
	void CreateComponentTypeStorage(T base, String typeName)
	{
		TypeNames.insert(std::pair<String, int>(typeName, typeCount));
		componentStorages.push_back(new ComponentTypeStorage<T>(typeCount));
		typeCount++;
	}

protected:
	

private:
	std::vector<iComponentStorage*> componentStorages;

	//std::vector<std::vector<Component*>*> ComponentsPerType;
	//std::vector<std::vector<bool>> IndexUsage;

	
	std::map<String, int> TypeNames;
	int typeCount = 0;

	//std::map<class, int> TypeNames;

	//std::vector<Component*> Components;
	//std::vector<bool> IndexUsage;
};


//template <class T = Component>