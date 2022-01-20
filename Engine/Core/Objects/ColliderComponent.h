#pragma once
#include "btBulletDynamicsCommon.h"
#include "BaseObject.h"

struct bulletObject {
    bool hit;
    btRigidBody* body;

    bulletObject(btRigidBody* b) : body(b), hit(false) {}
    bulletObject() : body(nullptr), hit(false) {}
    ~bulletObject() { delete body; }
};

class ColliderComponent : public BaseObject
{
public:
    bool collisionCheck(btManifoldPoint& cp, const btCollisionObject* obj1, int id1, int index1, const btCollisionObject* obj2, int id2, int index2);

private:
    
};
