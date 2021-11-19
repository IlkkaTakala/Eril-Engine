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
	//Get Pointers to ECS managers
	EntityManager* entityManager = IECS::GetEntityManager();
	ComponentManager* componentManager = IECS::GetComponentManager();

	//Create Component Storages:
	PositionComponent pc;
	componentManager->CreateComponentTypeStorage(pc, "PositionComponent");


	for (int i = 0; i < 2; i++)
	{
		Entity* entity = entityManager->AddEntity();
		Component* component = entityManager->CreateNewComponentToEntity(entity->GetID(), "PositionComponent");
		if (component != nullptr)
		{
			PositionComponent* posComponent = static_cast<PositionComponent*>(entityManager->GetComponentFromEntity(entity->GetID(), "PositionComponent"));
			posComponent->X = 100.0f * i;
			posComponent->Y = 100.0f * i;
			posComponent->Z = 100.0f * i;
			Console::Log(std::to_string(posComponent->Z) + "," + std::to_string(posComponent->Y) + "," + std::to_string(posComponent->Z));
		}
	}

	//Create new System and set it to handle entities with PositionComponents
	SystemsManager* systemsManager = IECS::GetSystemsManager();
	MovementSystem* movementSystem = new MovementSystem(*entityManager);
	int systemIndex = systemsManager->AddSystem(movementSystem);
	systemsManager->AddWantedComponentType(systemIndex, "PositionComponent");
	
}

void ECSTesting::Tick(float deltaTime)
{
	//STUFF
}
