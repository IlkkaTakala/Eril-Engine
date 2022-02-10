#include <vector>
#include <glad/gl.h>
#include <Physics/BulletPhysics.h>
#include "BulletObject.h"

btDynamicsWorld* world;
btDispatcher* dispatcher;
btCollisionConfiguration* collisionConfig;
btBroadphaseInterface* broadphase;
btConstraintSolver* solver;
std::vector<bulletObject*> bodies;

btRigidBody* Physics::addSphere(float rad, float x, float y, float z, float mass)
{
    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(x, y, z));
    btSphereShape* sphere = new btSphereShape(rad);
    btVector3 inertia(0, 0, 0);
    if (mass != 0.0)
        sphere->calculateLocalInertia(mass, inertia);

    btMotionState* motion = new btDefaultMotionState(t);
    btRigidBody::btRigidBodyConstructionInfo info(mass, motion, sphere, inertia);
    btRigidBody* body = new btRigidBody(info);
    world->addRigidBody(body);
    //bodies.push_back(new bulletObject(body, 0, 1.0, 0.0, 0.0));
    //body->setUserPointer(bodies[bodies.size() - 1]);
    return body;
}


btRigidBody* Physics::addCylinder(float d, float h, float x, float y, float z, float mass)
{
    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(x, y, z));
    btCylinderShape* sphere = new btCylinderShape(btVector3(d / 2.0, h / 2.0, d / 2.0));
    btVector3 inertia(0, 0, 0);
    if (mass != 0.0)
        sphere->calculateLocalInertia(mass, inertia);

    btMotionState* motion = new btDefaultMotionState(t);
    btRigidBody::btRigidBodyConstructionInfo info(mass, motion, sphere, inertia);
    btRigidBody* body = new btRigidBody(info);
    world->addRigidBody(body);
    /*bodies.push_back(new bulletObject(body, 1, 0.0, 1.0, 0.0));
    body->setUserPointer(bodies[bodies.size() - 1]);*/
    return body;
}


btRigidBody* Physics::addCone(float d, float h, float x, float y, float z, float mass)
{
    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(x, y, z));
    btConeShape* sphere = new btConeShape(d, h);
    btVector3 inertia(0, 0, 0);
    if (mass != 0.0)
        sphere->calculateLocalInertia(mass, inertia);

    btMotionState* motion = new btDefaultMotionState(t);
    btRigidBody::btRigidBodyConstructionInfo info(mass, motion, sphere, inertia);
    btRigidBody* body = new btRigidBody(info);
    world->addRigidBody(body);
    /*bodies.push_back(new bulletObject(body));
    body->setUserPointer(bodies[bodies.size() - 1]);*/
    return body;
}


btRigidBody* Physics::addBox(float width, float height, float depth, float x, float y, float z, float mass, int type)
{
    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(x, y, z));
    btBoxShape* box = new btBoxShape(btVector3(width / 2.0, height / 2.0, depth / 2.0));
    btVector3 inertia(0, 0, 0);
    if (mass != 0.0)
        box->calculateLocalInertia(mass, inertia);

    btMotionState* motion = new btDefaultMotionState(t);
    btRigidBody::btRigidBodyConstructionInfo info(mass, motion, box, inertia);
    btRigidBody* body = new btRigidBody(info);
    //body->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
    world->addRigidBody(body);
    /*bodies.push_back(new bulletObject(body));
    body->setUserPointer(bodies[bodies.size() - 1]);*/
    return body;
}


void Physics::RemoveBody(btRigidBody*& body)
{
    world->removeRigidBody(body);
    delete body;
    body = nullptr;
}

void Physics::Destroy()
{
    delete world;
    world = nullptr;

    delete solver;
    solver = nullptr;

    delete broadphase;
    broadphase = nullptr;

    delete dispatcher;
    dispatcher = nullptr;

    delete collisionConfig;
    collisionConfig = nullptr;

}

void Physics::init()
{
    collisionConfig = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfig);
    broadphase = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver();
    world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfig);
    world->setGravity(btVector3(0, -10, 0));
}

btDynamicsWorld* Physics::GetWorld()
{
    return world;
}

bulletObject* Physics::MakeRigidBoby(AABB box, Vector pos, float mass, int type)
{
    return new bulletObject(Physics::addBox(box.maxs.X - box.mins.X, box.maxs.Z - box.mins.Z, box.maxs.Y - box.mins.Y, pos.X, pos.Y, pos.Z, mass, type));
}

bool Physics::callbackFunc(btManifoldPoint& cp, const btCollisionObject* obj1, int id1, int index1, const btCollisionObject* obj2, int id2, int index2)
{
    ((bulletObject*)obj1->getUserPointer())->hit = true;

    ((bulletObject*)obj2->getUserPointer())->hit = true;
    return false;
}
