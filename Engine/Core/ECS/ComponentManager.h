#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021
*/

#include <vector>
#include <map>
#include "Core.h"
#include "Component.h"


template <class T>
class ComponentArray
{
public:
	ComponentArray() {};
	~ComponentArray() {};

	T& at(int i) { return Components.at(i); }

	std::vector<T> Components;
};

class ComponentManager
{
	friend class ECSWorldManager;
	friend class EntityManager;

public:
	ComponentManager();
	~ComponentManager();

	template <typename T>
	int AddComponent(T& component, String typeName);
	Component* GetComponent(int id, int type);
	bool RemoveComponent(int id, int type);
	int GetTypeByName(String typeName);

protected:
	

private:

	std::vector<ComponentArray<Component>*> ComponentsPerType;
	std::vector<std::vector<bool>> IndexUsage;

	
	std::map<String, int> TypeNames;
	int typeCount = 0;

	//std::vector<Component*> Components;
	//std::vector<bool> IndexUsage;
};


//template <class T = Component>