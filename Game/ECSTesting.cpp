/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021
*/

#include "ECSTesting.h"
#include <Interface/IECS.h>

#include "Components/PositionComponent.h"
#include "Systems/MovementSystem.h"

ECSTesting::ECSTesting()
{
	Console::Log("ECSTesting");
	EntityManager* ECSEntityManager = IECS::GetEntityManager();
	ComponentManager* componentManager = IECS::GetComponentManager();

	PositionComponent component;
	componentManager->CreateComponentTypeStorage(component, "PositionComponent");

	Entity* e1 = ECSEntityManager->AddEntity();
	int cID = ECSEntityManager->CreateNewComponentToEntity(e1->GetID(), "PositionComponent");
	Console::Log(std::to_string(cID));
	
	//Component* comp;
	// (e1->GetID(), "PositionComponent");
	PositionComponent* pos = nullptr;
	pos = static_cast<PositionComponent*>(ECSEntityManager->GetComponentFromEntity(e1->GetID(), "PositionComponent"));
	

	const void* address = static_cast<const void*>(pos);
	std::stringstream ss;
	ss << address;
	std::string name = ss.str();


	if (pos != nullptr)
	{
		pos->x += 1.0f;
		pos->y += 1.0f;
		pos->z += 1.0f;
		Console::Log(name + ": " + std::to_string(pos->x) + "," + std::to_string(pos->y) + "," + std::to_string(pos->z));
	}
	
	Entity* e2 = ECSEntityManager->AddEntity();
	cID = ECSEntityManager->CreateNewComponentToEntity(e2->GetID(), "PositionComponent");
	Console::Log(std::to_string(cID));

	pos = static_cast<PositionComponent*>(ECSEntityManager->GetComponentFromEntity(e2->GetID(), "PositionComponent"));


	address = static_cast<const void*>(pos);
	std::stringstream ss2;
	ss2 << address;
	name = ss2.str();

	if (pos != nullptr)
	{
		pos->x += 20.0f;
		pos->y += 20.0f;
		pos->z += 20.0f;

		Console::Log(name + ": " + std::to_string(pos->x) + "," + std::to_string(pos->y) + "," + std::to_string(pos->z));
	}
	
	Entity* e3 = ECSEntityManager->AddEntity();
	cID = ECSEntityManager->CreateNewComponentToEntity(e3->GetID(), "PositionComponent");
	pos = static_cast<PositionComponent*>(ECSEntityManager->GetComponentFromEntity(e3->GetID(), "PositionComponent"));
	Console::Log(std::to_string(cID));

	if (pos != nullptr)
	{
		pos->x += 300.0f;
		pos->y += 300.0f;
		pos->z += 300.0f;

		Console::Log(std::to_string(pos->x) + "," + std::to_string(pos->y) + "," + std::to_string(pos->z));
	}

	Entity* e4 = ECSEntityManager->AddEntity();
	cID = ECSEntityManager->CreateNewComponentToEntity(e4->GetID(), "PositionComponent");
	pos = static_cast<PositionComponent*>(ECSEntityManager->GetComponentFromEntity(e4->GetID(), "PositionComponent"));
	Console::Log(std::to_string(cID));

	if (pos != nullptr)
	{
		pos->x += 4000.0f;
		pos->y += 4000.0f;
		pos->z += 4000.0f;

		Console::Log(std::to_string(pos->x) + "," + std::to_string(pos->y) + "," + std::to_string(pos->z));
	}



	SystemsManager* ECSSystemsManager = IECS::GetSystemsManager();
	MovementSystem* movementSystem = new MovementSystem(*ECSEntityManager);
	int systemIndex = ECSSystemsManager->AddSystem(movementSystem);
	ECSSystemsManager->AddWantedComponentType(systemIndex, "PositionComponent");
	
}

void ECSTesting::Tick(float deltaTime)
{
	//STUFF
}
