#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021
*/

#include <vector>
#include <map>
#include "Core.h"
#include "Component.h"

class ComponentManager
{
	friend class ECSWorldManager;
	friend class EntityManager;

public:
	ComponentManager();
	~ComponentManager();

	int AddComponent(Component* component, String typeName);
	Component* GetComponent(int id, int type);
	bool RemoveComponent(int id, int type);
	int GetTypeByName(String typeName);

protected:
	

private:

	std::vector<std::vector<Component*>*> ComponentsPerType;
	std::vector<std::vector<bool>> IndexUsage;

	
	std::map<String, int> TypeNames;
	int typeCount = 0;

	//std::vector<Component*> Components;
	//std::vector<bool> IndexUsage;
};


//template <class T = Component>