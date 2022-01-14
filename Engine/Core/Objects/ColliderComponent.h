#pragma once
#include "SceneComponent.h"
#include "Physics/BulletPhysics.h"
#include <Physics/BulletObject.h>
#include "btBulletCollisionCommon.h"

class ColliderComponent : public SceneComponent
{
    ColliderComponent();
    ~ColliderComponent();
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionObject* collisionObject;


private:
    bulletObject collider;

};

