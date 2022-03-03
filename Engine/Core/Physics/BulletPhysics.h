#pragma once
//http://www.youtube.com/user/thecplusplusguy
//bullet physics tutorial 3, collision callback
#include <btBulletDynamicsCommon.h>
#include "BulletObject.h"
#include <Core.h>

namespace Physics 
{

btRigidBody* addSphere(float radius, float x, float y, float z, float mass, btMotionState* state);
btRigidBody* addSphereStatic(float radius, float x, float y, float z, btMotionState* state);

btRigidBody* addCylinder(float width, float height, float depth, float x, float y, float z, float mass, btMotionState* state);
btRigidBody* addCylinderStatic(float width, float height, float depth, float x, float y, float z, btMotionState* state);

btRigidBody* addCone(float radius, float height, float x, float y, float z, float mass, btMotionState* state);
btRigidBody* addConeStatic(float radius, float height, float x, float y, float z, btMotionState* state);

btRigidBody* addBox(float width, float height, float depth, float x, float y, float z, float mass, btMotionState* state);
btRigidBody* addBoxStatic(float width, float height, float depth, float x, float y, float z, btMotionState* state);

btRigidBody* addCapsule(float radius, float height, float x, float y, float z, float mass, btMotionState* state);
btRigidBody* addCapsuleStatic(float radius, float height, float x, float y, float z, btMotionState* state);

void ForceUpdate(btRigidBody* body);
void RemoveBody(btRigidBody*& body);

void Destroy();

void init();

btDynamicsWorld* GetWorld();

bool callbackFunc(btManifoldPoint& cp, const btCollisionObject* obj1, int id1, int index1, const btCollisionObject* obj2, int id2, int index2);

}
