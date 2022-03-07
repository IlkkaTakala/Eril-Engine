#pragma once
//http://www.youtube.com/user/thecplusplusguy
//bullet physics tutorial 3, collision callback
#include <btBulletDynamicsCommon.h>
#include "BulletObject.h"
#include <Core.h>

namespace Physics 
{


btRigidBody* addSphere(float rad, float x, float y, float z, float mass);

btRigidBody* addCylinder(float d, float h, float x, float y, float z, float mass);

btRigidBody* addCone(float d, float h, float x, float y, float z, float mass);

btRigidBody* addBox(float width, float height, float depth, float x, float y, float z, float mass, btMotionState* state);
btRigidBody* addBoxStatic(float width, float height, float depth, float x, float y, float z, btMotionState* state);

void ForceUpdate(btRigidBody* body);
void RemoveBody(btRigidBody*& body);

void Destroy();

void init();

btDynamicsWorld* GetWorld();

bool callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1, const btCollisionObjectWrapper* obj2, int id2, int index2);


bool LineTraceSingle(const Vector& start, const Vector& end, Vector& hitLocation, Vector& hitNormal);
}
