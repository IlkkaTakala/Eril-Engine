/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021
*/

#include "ECSTesting.h"

ECSTesting::ECSTesting()
{
	
	/*
	ComponentManager& ECSComponentManager = ECSWorld.GetComponentManager();
	int componentID = ECSComponentManager.AddComponent(new PositionComponent, "PositionComponent");
	PositionComponent* component = static_cast<PositionComponent*>(ECSComponentManager.GetComponent(componentID));
	component->x = 10.0f;
	component->y = 10.0f;
	component->z = 10.0f;

	testID = componentID;

	printf("Component MADE!\n");
	*/

	/*
	EntityManager& ECSEntityManager = ECSWorld.GetEntityManager();
	Entity* e1 = ECSEntityManager.AddEntity();

	if (!ECSEntityManager.AddComponentToEntity(e1->GetID(), new PositionComponent, "PositionComponent"))
	{
		printf("Could not add the Component!!");
	}
	entityID = e1->GetID();
	*/

	EntityManager& ECSEntityManager = ECSWorld.GetEntityManager();
	
	Entity* e1 = ECSEntityManager.AddEntity();
	PositionComponent component;

	ECSEntityManager.AddComponentToEntity(e1->GetID(), component, "PositionComponent");
	PositionComponent* pos = static_cast<PositionComponent*>(ECSEntityManager.GetComponentFromEntity(e1->GetID(), "PositionComponent"));
	if (pos != nullptr)
	{
		pos->x += 1.0f;
		pos->y += 1.0f;
		pos->z += 1.0f;

		printf("%f,%f,%f\n", pos->x, pos->y, pos->z);
	}

	Entity* e2 = ECSEntityManager.AddEntity();
	ECSEntityManager.AddComponentToEntity(e2->GetID(), component, "PositionComponent");
	pos = static_cast<PositionComponent*>(ECSEntityManager.GetComponentFromEntity(e2->GetID(), "PositionComponent"));
	if (pos != nullptr)
	{
		pos->x += 20.0f;
		pos->y += 20.0f;
		pos->z += 20.0f;

		printf("%f,%f,%f\n", pos->x, pos->y, pos->z);
	}
	
	Entity* e3 = ECSEntityManager.AddEntity();
	ECSEntityManager.AddComponentToEntity(e3->GetID(), component, "PositionComponent");
	pos = static_cast<PositionComponent*>(ECSEntityManager.GetComponentFromEntity(e3->GetID(), "PositionComponent"));
	if (pos != nullptr)
	{
		pos->x += 300.0f;
		pos->y += 300.0f;
		pos->z += 300.0f;

		printf("%f,%f,%f\n", pos->x, pos->y, pos->z);
	}

	Entity* e4 = ECSEntityManager.AddEntity();
	ECSEntityManager.AddComponentToEntity(e4->GetID(), component, "PositionComponent");
	pos = static_cast<PositionComponent*>(ECSEntityManager.GetComponentFromEntity(e4->GetID(), "PositionComponent"));
	if (pos != nullptr)
	{
		pos->x += 4000.0f;
		pos->y += 4000.0f;
		pos->z += 4000.0f;

		printf("%f,%f,%f\n", pos->x, pos->y, pos->z);
	}
	


	SystemsManager& ECSSystemsManager = ECSWorld.GetSystemsManager();
	MovementSystem* movementSystem = new MovementSystem(ECSEntityManager);
	int systemIndex = ECSSystemsManager.AddSystem(movementSystem);
	ECSSystemsManager.AddWantedComponentType(systemIndex, "PositionComponent");

}

void ECSTesting::Tick(float deltaTime)
{
	ECSWorld.Update(deltaTime);

	/*
	ComponentManager& ECSComponentManager = ECSWorld.GetComponentManager();
	PositionComponent* c = static_cast<PositionComponent*>(ECSComponentManager.GetComponent(testID));
	if (c != nullptr)
	{
		c->x += 1.0f;
		c->y += 1.0f;
		c->z += 1.0f;

		printf("%f,%f,%f\n", c->x, c->y, c->z);
	}
	else
	{
		printf("Component NOT FOUND!");
	}
	*/
	
	/*
	EntityManager& ECSEntityManager = ECSWorld.GetEntityManager();
	Entity* e1 = ECSEntityManager.GetEntity(entityID);
	PositionComponent* pos = static_cast<PositionComponent*>(ECSEntityManager.GetComponentFromEntity(e1->GetID(), "PositionComponent"));
	if (pos != nullptr)
	{
		pos->x += 1.0f;
		pos->y += 1.0f;
		pos->z += 1.0f;

		printf("%f,%f,%f\n", pos->x, pos->y, pos->z);
	}
	else
	{
		printf("Component NOT FOUND!");
	}
	*/
}
