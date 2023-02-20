#pragma once
#include <btBulletDynamicsCommon.h>

struct bulletObject {
    bool hit;
    btRigidBody* body;

    bulletObject(btRigidBody* b) : body(b), hit(false) {}
    bulletObject() : body(nullptr), hit(false) {}
    ~bulletObject()
    {
        delete body;
    }
};
