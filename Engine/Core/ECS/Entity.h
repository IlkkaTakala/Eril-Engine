#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021, Last Edited by RabbitTortoise 24.11.2021
*/

#include <map>

class Entity
{
public:

	Entity() { ID = -1;  bEnabled = false; bProtected = false; };
	Entity(int id, bool enabled = true) : ID(id), bEnabled(enabled), bProtected(false){}
	~Entity() {}

	int GetID() { return ID; }
	bool GetEnabled() { return bEnabled; }
	bool GetProtected() { return bProtected; }

	/// <summary>
	/// Return handles to every component the entity has.
	/// </summary>
	/// <returns>Map, First: Component ID, Second: Component Type.</returns>
	std::map<int, int>& GetComponents() { return Components; }

	void SetID(int id) { ID = id; }
	void SetEnabled(bool enabled) { bEnabled = enabled; }
	void SetProtected(bool isProtected) { bProtected = isProtected; }

protected:
	int ID;
	bool bEnabled;
	bool bProtected;

	/// <summary>
	/// First: Component ID, Second: Component Type
	/// </summary>
	std::map<int, int> Components;
};

