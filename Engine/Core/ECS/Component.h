#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021 Last Edited by RabbitTortoise 24.11.2021
*/



/// <summary>
/// All components inherit from this class. ID and Type are assinged by the system and shouldn't be touched after they have been set.
/// If the component has been disabled, don't use it in your systems.
/// When inheriting this class, always make at least a copy-constructor that also calls this class's copy constructor. Even better if you also make move constructor and a operator= override.
/// Failing to provide correct constructors causes undefined behaviour.
/// </summary>
class Component
{
public:
	Component() {}
	Component(Component const& c) : ID(c.ID), Type(c.Type), Disabled(c.Disabled) {}
	Component(Component &&c) noexcept : ID(std::move(c.ID)), Type(std::move(c.Type)), Disabled(std::move(c.Disabled)) {}

	Component& operator=(const Component& c) { ID = c.ID; Type = c.Type; Disabled = c.Disabled; return *this; }

	~Component() {}

	int GetID() { return ID; }
	int GetType() { return Type; }
	bool GetDisabled() { return Disabled; }

	void SetID(int id) { ID = id; }
	void SetType(int type) { Type = type; }
	void SetDisabled() { Disabled = true; }

protected:
	int ID = -1;
	int Type = -1;
	bool Disabled = false;
};

/* Example of a correctly inherited Component.
class PositionComponent : public Component
{
public:
	PositionComponent() : Component() { Console::Log("PositionComponent()"); }
	PositionComponent(const PositionComponent &c) : Component(static_cast<Component>(c)), X(c.X), Y(c.Y), Z(c.Z) {}
	PositionComponent(PositionComponent &&c) noexcept : Component(std::move(c)), X(std::move(c.X)), Y(std::move(c.Y)), Z(std::move(c.Z)) {}

	PositionComponent& operator=(const PositionComponent& c) { X = c.X; Y = c.Y; Z = c.Z; ID = c.ID; Type = c.Type; Disabled = c.Disabled; return *this; }

	float X = 0.0f;
	float Y = 0.0f;
	float Z = 0.0f;
};
*/