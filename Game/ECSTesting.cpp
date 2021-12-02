/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021, Last Edited by RabbitTortoise 24.11.2021
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
	componentManager->CreateComponentTypeStorage<PositionComponent>("PositionComponent");


	int debugSize = 10;

	for (int i = 0; i < debugSize; i++)
	{
		Entity* entity = entityManager->AddEntity();
		PositionComponent* pos = entityManager->AddComponentToEntity<PositionComponent>(entity->GetID(), "PositionComponent");
		if (pos != nullptr)
		{
			pos->X += 100.0f + (i + 1);
			pos->Y += 100.0f + (i + 1);
			pos->Z += 100.0f + (i + 1);
			//Console::Log(std::to_string(pos->GetID()) + ": " + std::to_string(pos->Z) + "," + std::to_string(pos->Y) + "," + std::to_string(pos->Z));
		}
	}
	/*

	for (int i = 0; i < debugSize/2; i++)
	{
		int eID = rand() % debugSize;
		if (entityManager->RemoveEntity(eID))
		{
			//Console::Log("Removed Entity and it's components" + std::to_string(eID));
		}
		else
		{
			//Console::Log("Unable to remove Entity " + std::to_string(eID));
		}
	}

	for (int i = 0; i < debugSize / 2; i++)
	{
		int eID = rand() % debugSize;
		if (entityManager->RemoveComponentFromEntity<PositionComponent>(eID, "PositionComponent"))
		{
			//Console::Log("Removed PositionComponent from Entity " + std::to_string(eID));
		}
		else
		{
			//Console::Log("Unable to remove PositionComponent from Entity " + std::to_string(eID));
		}
	}

	for (int i = 0; i < debugSize; i++)
	{
		Entity* entity = entityManager->AddEntity();
		PositionComponent* pos = entityManager->AddComponentToEntity<PositionComponent>(entity->GetID(), "PositionComponent");
		if (pos != nullptr)
		{
			pos->X += 100.0f * (i + 1) * debugSize;
			pos->Y += 100.0f * (i + 1) * debugSize;
			pos->Z += 100.0f * (i + 1) * debugSize;
			//Console::Log(std::to_string(pos->GetID()) + ": " + std::to_string(pos->Z) + "," + std::to_string(pos->Y) + "," + std::to_string(pos->Z));
		}
	}

	for (int i = 0; i < debugSize / 2; i++)
	{
		int eID = rand() % debugSize;
		if (entityManager->RemoveEntity(eID))
		{
			//Console::Log("Removed Entity and it's components" + std::to_string(eID));
		}
		else
		{
			//Console::Log("Unable to remove Entity " + std::to_string(eID));
		}
	}

	for (int i = 0; i < debugSize / 2; i++)
	{
		int eID = rand() % debugSize;
		if (entityManager->RemoveComponentFromEntity<PositionComponent>(eID, "PositionComponent"))
		{
			//Console::Log("Removed PositionComponent from Entity " + std::to_string(eID));
		}
		else
		{
			//Console::Log("Unable to remove PositionComponent from Entity " + std::to_string(eID));
		}
	}
	*/


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
