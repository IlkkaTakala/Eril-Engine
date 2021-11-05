#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021
*/

#include <vector>

class Entity
{
	friend class EntityManager;
public:

	Entity() { ID = -1;  bEnabled = false; };
	Entity(int id, bool enabled = true) : ID(id), bEnabled(enabled) {}
	~Entity() {}

	int GetID() { return ID; }
	bool GetEnabled() { return bEnabled; }
	std::vector<int>& GetComponents() { return Components; }
	std::vector<int>& GetComponentTypes() { return ComponentTypes; }

	void SetEnabled(bool enabled) { bEnabled = enabled; }
	void SetID(int id) { ID = id; }

protected:
	int ID;
	bool bEnabled;
	std::vector<int> Components;
	std::vector<int> ComponentTypes;
};

