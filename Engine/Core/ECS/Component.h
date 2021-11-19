#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021 Last Edited by RabbitTortoise 18.11.2021
*/



/// <summary>
/// All components inherit from this class. ID and Type are assinged by the system and shouldn't be touched after they have been set.
/// Init function is called when the component is added to the system so you should implement it in your own component type to set default values.
/// If the component has been disabled, discard it in your systems.
/// </summary>
class Component
{
public:
	Component() {}
	Component(Component const& c) { ID = c.ID; Type = c.Type; Disabled = c.Disabled; }

	~Component() {}

	int GetID() { return ID; }
	int GetType() { return Type; }

	void SetID(int id) { ID = id; }
	void SetType(int type) { Type = type; }
	void SetDisabled() { Disabled = true; }

	Component* GetPointer() { return this; }

private:
	int ID = -1;
	int Type = -1;
	bool Disabled = false;
};

