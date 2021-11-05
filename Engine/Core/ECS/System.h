#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021
*/

#include <vector>
#include "ECS/EntityManager.h"

class System
{
	friend class SystemsManager;

public:
	System(EntityManager& entityManager) : WorldEntityManager(entityManager) { bEnabled = false; }
	~System() {}
	virtual void Update(float deltaTime, std::vector<int> entities) = 0;
	void SetEnabled(bool enabled) { bEnabled = enabled; }
	
protected:
	bool bEnabled;
	EntityManager& WorldEntityManager;
	std::vector<int> TypesWanted;
	std::vector<int> TypesNotAllowed;

};

