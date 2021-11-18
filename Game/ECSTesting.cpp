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
	
	EntityManager* ECSEntityManager = IECS::GetEntityManager();
	ComponentManager* componentManager = IECS::GetComponentManager();

	PositionComponent component;
	componentManager->CreateComponentTypeStorage(component, "PositionComponent");

	Entity* e1 = ECSEntityManager->AddEntity();
	ECSEntityManager->AddComponentToEntity(e1->GetID(), component, "PositionComponent");
	
	//Component* comp;
	// (e1->GetID(), "PositionComponent");
	PositionComponent* pos = nullptr;
	pos = static_cast<PositionComponent*>(ECSEntityManager->GetComponentFromEntity(e1->GetID(), "PositionComponent"));
	// = static_cast<PositionComponent*>(comp)
	if (pos != nullptr)
	{
		pos->x += 1.0f;
		pos->y += 1.0f;
		pos->z += 1.0f;
		Console::Log( std::to_string(pos->x) + "," + std::to_string(pos->y) + "," + std::to_string(pos->z));
	}
	
	Entity* e2 = ECSEntityManager->AddEntity();
	ECSEntityManager->AddComponentToEntity(e2->GetID(), component, "PositionComponent");
	pos = static_cast<PositionComponent*>(ECSEntityManager->GetComponentFromEntity(e2->GetID(), "PositionComponent"));
	if (pos != nullptr)
	{
		pos->x += 20.0f;
		pos->y += 20.0f;
		pos->z += 20.0f;

		Console::Log(std::to_string(pos->x) + "," + std::to_string(pos->y) + "," + std::to_string(pos->z));
	}
	
	Entity* e3 = ECSEntityManager->AddEntity();
	ECSEntityManager->AddComponentToEntity(e3->GetID(), component, "PositionComponent");
	pos = static_cast<PositionComponent*>(ECSEntityManager->GetComponentFromEntity(e3->GetID(), "PositionComponent"));
	if (pos != nullptr)
	{
		pos->x += 300.0f;
		pos->y += 300.0f;
		pos->z += 300.0f;

		Console::Log(std::to_string(pos->x) + "," + std::to_string(pos->y) + "," + std::to_string(pos->z));
	}

	Entity* e4 = ECSEntityManager->AddEntity();
	ECSEntityManager->AddComponentToEntity(e4->GetID(), component, "PositionComponent");
	pos = static_cast<PositionComponent*>(ECSEntityManager->GetComponentFromEntity(e4->GetID(), "PositionComponent"));
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
