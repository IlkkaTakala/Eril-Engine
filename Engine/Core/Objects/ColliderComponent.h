#pragma once
#include "SceneComponent.h"
#include "Physics/BulletPhysics.h"
#include <BulletObject.h>

class ColliderComponent :
    public SceneComponent
{
    // rigitbody

private:
    bulletObject collider;


};

