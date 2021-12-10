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


btRigidBody* Physics::addBox(float width, float height, float depth, float x, float y, float z, float mass)
{
    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(x, y, z));
    btBoxShape* sphere = new btBoxShape(btVector3(width / 2.0, height / 2.0, depth / 2.0));
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

void Physics::init()
{
    collisionConfig = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfig);
    broadphase = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver();
    world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfig);
    world->setGravity(btVector3(0, -10, 0));

    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(0, 0, 0));
    btStaticPlaneShape* plane = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
    btMotionState* motion = new btDefaultMotionState(t);
    btRigidBody::btRigidBodyConstructionInfo info(0.0, motion, plane);
    btRigidBody* body = new btRigidBody(info);
    world->addRigidBody(body);
    /*  bodies.push_back(new bulletObject(body, 4, 0.8, 0.8, 0.8));*/
    /*  body->setUserPointer(bodies[bodies.size() - 1]);*/

    addSphere(1.0, 0, 20, 0, 1.0);

    glClearColor(0, 0, 0, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
}

btDynamicsWorld* Physics::GetWorld()
{
    return world;
}

bulletObject* Physics::MakeRigidBoby(AABB box)
{
    return new bulletObject(Physics::addBox(box.maxs.X - box.mins.X, box.maxs.Z - box.mins.Z, box.maxs.Y - box.mins.Y, 0, 0, 0, 10));
}
