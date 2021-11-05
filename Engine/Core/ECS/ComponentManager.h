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
	~ComponentManager();

	int AddComponent(Component* component, String typeName);
	Component* GetComponent(int id);
	bool RemoveComponent(int id);
	int GetTypeByName(String typeName);

protected:
	

private:
	std::vector<Component*> Components;
	std::vector<bool> IndexUsage;
	std::map<String, int> TypeNames;
	int typeCount = 0;
};