//http://www.youtube.com/user/thecplusplusguy
//bullet physics tutorial 3, collision callback
#include <iostream>
#include "camera.h"
#include <vector>
#include <btBulletDynamicsCommon.h>
#include <glad/gl.h>
#include "../../../BulletObject.h"


btRigidBody* addSphere(float rad, float x, float y, float z, float mass);

btRigidBody* addCylinder(float d, float h, float x, float y, float z, float mass);

btRigidBody* addCone(float d, float h, float x, float y, float z, float mass);

btRigidBody* addBox(float width, float height, float depth, float x, float y, float z, float mass);

//
//void init(float angle)
//{
//    collisionConfig = new btDefaultCollisionConfiguration();
//    dispatcher = new btCollisionDispatcher(collisionConfig);
//    broadphase = new btDbvtBroadphase();
//    solver = new btSequentialImpulseConstraintSolver();
//    world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfig);
//    world->setGravity(btVector3(0, -10, 0));
//
//    btTransform t;
//    t.setIdentity();
//    t.setOrigin(btVector3(0, 0, 0));
//    btStaticPlaneShape* plane = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
//    btMotionState* motion = new btDefaultMotionState(t);
//    btRigidBody::btRigidBodyConstructionInfo info(0.0, motion, plane);
//    btRigidBody* body = new btRigidBody(info);
//    world->addRigidBody(body);
//  /*  bodies.push_back(new bulletObject(body, 4, 0.8, 0.8, 0.8));*/
//  /*  body->setUserPointer(bodies[bodies.size() - 1]);*/
//
//    addSphere(1.0, 0, 20, 0, 1.0);
//
//    glClearColor(0, 0, 0, 1);
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    glMatrixMode(GL_MODELVIEW);
//    glEnable(GL_DEPTH_TEST);
//}



bool callbackFunc(btManifoldPoint& cp, const btCollisionObject* obj1, int id1, int index1, const btCollisionObject* obj2, int id2, int index2)
{
    ((bulletObject*)obj1->getUserPointer())->hit = true;

    ((bulletObject*)obj2->getUserPointer())->hit = true;
    return false;
}