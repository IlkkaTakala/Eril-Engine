#include "ColliderComponent.h"

bool ColliderComponent::collisionCheck(btManifoldPoint& cp, const btCollisionObject* obj1, int id1, int index1, const btCollisionObject* obj2, int id2, int index2)
{
	((bulletObject*)obj1->getUserPointer())->hit = true;
	((bulletObject*)obj2->getUserPointer())->hit = true;
	return false;
}
