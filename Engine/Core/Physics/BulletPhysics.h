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

btRigidBody* addBox(float width, float height, float depth, float x, float y, float z, float mass);


void init();

btDynamicsWorld* GetWorld();

bulletObject* MakeRigidBoby(AABB box, Vector pos);

bool callbackFunc(btManifoldPoint& cp, const btCollisionObject* obj1, int id1, int index1, const btCollisionObject* obj2, int id2, int index2)
{
    ((bulletObject*)obj1->getUserPointer())->hit = true;

    ((bulletObject*)obj2->getUserPointer())->hit = true;
    return false;
}

}
