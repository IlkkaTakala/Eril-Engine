#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021, Last Edited by RabbitTortoise 24.11.2021
*/

#include <vector>
#include "ECS/EntityManager.h"

class System
{
	friend class SystemsManager;

public:
	System(EntityManager& entityManager) : WorldEntityManager(entityManager) { bEnabled = false; }
	~System() {}

	

	/// <summary>
	/// Set System to be enabled or disabled.
	/// </summary>
	/// <param name="enabled">True/False</param>
	void SetEnabled(bool enabled) { bEnabled = enabled; }
	
protected:
	bool bEnabled;
	EntityManager& WorldEntityManager;
	std::vector<int> TypesWanted;
	std::vector<int> TypesNotAllowed;
};


class IEntityQuerySystem : public System
{
public:
	IEntityQuerySystem(EntityManager& entityManager) : System(entityManager) {}

	/// <summary>
	/// This is called once per frame.
	/// </summary>
	/// <param name="deltaTime">Time between frames</param>
	/// <param name="entities">Entities to go through</param>
	virtual void Update(float deltaTime, std::vector<int> entities) = 0;
};
