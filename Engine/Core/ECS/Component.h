#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021
*/

class Component
{
	friend class ComponentManager;
public:
	Component() { ID = -1; Type = -1; }
	Component(int id, int type) : ID(id), Type(type) {}
	~Component() {}

	int GetID() { return ID; }
	int GetType() { return Type; }

	void SetType(int type) { Type = type; }
	void SetID(int id) { ID = id; }

	void Delete() { Valid = false; }
	bool IsValid() { return Valid; }

private:
	int ID;
	int Type;
	bool Valid = true;
};

