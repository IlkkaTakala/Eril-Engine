#include <vector>
#include <glad/gl.h>
#include <Physics/BulletPhysics.h>
#include "BulletObject.h"
#include <Objects/ColliderComponent.h>

btDynamicsWorld* world;
btDispatcher* dispatcher;
btCollisionConfiguration* collisionConfig;
btBroadphaseInterface* broadphase;
btConstraintSolver* solver;
std::vector<bulletObject*> bodies;

btRigidBody* Physics::addSphere(float radius, float x, float y, float z, float mass, btMotionState* state)
{
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(x, y, z));
	t.setRotation(btQuaternion(0, 0, 0, 1));
	btSphereShape* sphere = new btSphereShape(radius);
	btVector3 inertia(0, 0, 0);
	if (mass != 0.0)
		sphere->calculateLocalInertia(mass, inertia);

	btRigidBody::btRigidBodyConstructionInfo info(mass, state, sphere, inertia);
	btRigidBody* body = new btRigidBody(info);
	body->setCollisionFlags(btCollisionObject::CF_DYNAMIC_OBJECT);
	world->addRigidBody(body);
	return body;
}

btRigidBody* Physics::addSphereStatic(float radius, float x, float y, float z, btMotionState* state)
{
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(x, y, z));
	t.setRotation(btQuaternion(0, 0, 0, 1));
	btSphereShape* sphere = new btSphereShape(radius);
	btVector3 inertia(0, 0, 0);

	btRigidBody::btRigidBodyConstructionInfo info(0, state, sphere, inertia);
	btRigidBody* body = new btRigidBody(info);
	body->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
	body->setAngularFactor(0.f);
	world->addRigidBody(body);
	return body;
}

btRigidBody* Physics::addCylinder(float width, float height, float depth, float x, float y, float z, float mass, btMotionState* state)
{
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(x, y, z));
	t.setRotation(btQuaternion(0, 0, 0, 1));
	btCylinderShape* cylinder = new btCylinderShape(btVector3(width / 2.f, height / 2.f, depth / 2.f));
	btVector3 inertia(0, 0, 0);
	if (mass != 0.0)
		cylinder->calculateLocalInertia(mass, inertia);

	btRigidBody::btRigidBodyConstructionInfo info(mass, state, cylinder, inertia);
	btRigidBody* body = new btRigidBody(info);
	body->setCollisionFlags(btCollisionObject::CF_DYNAMIC_OBJECT);
	world->addRigidBody(body);
	return body;
}

btRigidBody* Physics::addCylinderStatic(float width, float height, float depth, float x, float y, float z, btMotionState* state)
{
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(x, y, z));
	t.setRotation(btQuaternion(0, 0, 0, 1));
	btCylinderShape* cylinder = new btCylinderShape(btVector3(width / 2.f, height / 2.f, depth / 2.f));
	btVector3 inertia(0, 0, 0);
	
	btRigidBody::btRigidBodyConstructionInfo info(0, state, cylinder, inertia);
	btRigidBody* body = new btRigidBody(info);
	body->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
	body->setAngularFactor(0.f);
	world->addRigidBody(body);
	return body;
}

btRigidBody* Physics::addCone(float radius, float height, float x, float y, float z, float mass, btMotionState* state)
{
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(x, y, z));
	t.setRotation(btQuaternion(0, 0, 0, 1));
	btConeShape* cone = new btConeShape(radius, height);
	btVector3 inertia(0, 0, 0);
	if (mass != 0.0)
		cone->calculateLocalInertia(mass, inertia);

	btRigidBody::btRigidBodyConstructionInfo info(mass, state, cone, inertia);
	btRigidBody* body = new btRigidBody(info);
	body->setCollisionFlags(btCollisionObject::CF_DYNAMIC_OBJECT);
	world->addRigidBody(body);
	return body;
}

btRigidBody* Physics::addConeStatic(float radius, float height, float x, float y, float z, btMotionState* state)
{
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(x, y, z));
	t.setRotation(btQuaternion(0, 0, 0, 1));
	btConeShape* cone = new btConeShape(radius, height);
	btVector3 inertia(0, 0, 0);
	
	btRigidBody::btRigidBodyConstructionInfo info(0, state, cone, inertia);
	btRigidBody* body = new btRigidBody(info);
	body->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
	body->setAngularFactor(0.f);
	world->addRigidBody(body);
	return body;
}

btRigidBody* Physics::addBox(float width, float height, float depth, float x, float y, float z, float mass, btMotionState* state)
{
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(x, y, z));
	t.setRotation(btQuaternion(0, 0, 0, 1));
	btBoxShape* box = new btBoxShape(btVector3(width / 2.f, height / 2.f, depth / 2.f));
	btVector3 inertia(0, 0, 0);
	if (mass != 0.0)
		box->calculateLocalInertia(mass, inertia);

	btRigidBody::btRigidBodyConstructionInfo info(mass, state, box, inertia);
	btRigidBody* body = new btRigidBody(info);
	body->setCollisionFlags(btCollisionObject::CF_DYNAMIC_OBJECT);
	world->addRigidBody(body);
	return body;
}

btRigidBody* Physics::addBoxStatic(float width, float height, float depth, float x, float y, float z, btMotionState* state)
{
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(x, y, z));
	t.setRotation(btQuaternion(0, 0, 0, 1));
	btBoxShape* box = new btBoxShape(btVector3(width / 2.f, height / 2.f, depth / 2.f));
	btVector3 inertia(0, 0, 0);

	btRigidBody::btRigidBodyConstructionInfo info(0, state, box, inertia);
	btRigidBody* body = new btRigidBody(info);
	body->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
	body->setAngularFactor(0.f);
	world->addRigidBody(body);
	return body;
}

btRigidBody* Physics::addCapsule(float radius, float height, float x, float y, float z, float mass, btMotionState* state)
{
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(x, y, z));
	t.setRotation(btQuaternion(0, 0, 0, 1));
	btCapsuleShape* capsule = new btCapsuleShape(radius, height);
	btVector3 inertia(0, 0, 0);
	if (mass != 0.0)
		capsule->calculateLocalInertia(mass, inertia);

	btRigidBody::btRigidBodyConstructionInfo info(mass, state, capsule, inertia);
	btRigidBody* body = new btRigidBody(info);
	body->setCollisionFlags(btCollisionObject::CF_DYNAMIC_OBJECT);
	world->addRigidBody(body);
	return body;
}

btRigidBody* Physics::addCapsuleStatic(float radius, float height, float x, float y, float z, btMotionState* state)
{
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(x, y, z));
	t.setRotation(btQuaternion(0, 0, 0, 1));
	btCapsuleShape* capsule = new btCapsuleShape(radius, height);
	btVector3 inertia(0, 0, 0);

	btRigidBody::btRigidBodyConstructionInfo info(0, state, capsule, inertia);
	btRigidBody* body = new btRigidBody(info);
	body->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
	body->setAngularFactor(0.f);
	world->addRigidBody(body);
	return body;
}

void Physics::ForceUpdate(btRigidBody* body)
{
	world->updateSingleAabb(body);
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
	gContactAddedCallback = Physics::callbackFunc;
}

btDynamicsWorld* Physics::GetWorld()
{
	return world;
}

bool Physics::callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1, const btCollisionObjectWrapper* obj2, int id2, int index2)
{
	((ColliderComponent*)obj1->getCollisionObject()->getUserPointer())->OnCollide();

	((ColliderComponent*)obj2->getCollisionObject()->getUserPointer())->OnCollide();

	return false;
}

bool Physics::LineTraceSingle(const Vector& start, const Vector& end, Vector& hitLocation, Vector& hitNormal)
{
	btVector3 rayFrom(start.X, start.Z, start.Y);
	btVector3 rayTo(end.X, end.Z, end.Y);
	struct	AllRayResultCallback : public btCollisionWorld::RayResultCallback
	{
		AllRayResultCallback(const btVector3& rayFromWorld, const btVector3& rayToWorld)
			:m_rayFromWorld(rayFromWorld),
			m_rayToWorld(rayToWorld)
		{
		}

		btVector3	m_rayFromWorld;//used to calculate hitPointWorld from hitFraction
		btVector3	m_rayToWorld;

		btVector3	m_hitNormalWorld;
		btVector3	m_hitPointWorld;

		virtual	btScalar	addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
		{

			//caller already does the filter on the m_closestHitFraction
			btAssert(rayResult.m_hitFraction <= m_closestHitFraction);

			m_closestHitFraction = rayResult.m_hitFraction;

			m_collisionObject = rayResult.m_collisionObject;
			if (normalInWorldSpace)
			{
				m_hitNormalWorld = rayResult.m_hitNormalLocal;
			}
			else
			{
				///need to transform normal into worldspace
				m_hitNormalWorld = m_collisionObject->getWorldTransform().getBasis() * rayResult.m_hitNormalLocal;
			}
			m_hitPointWorld.setInterpolate3(m_rayFromWorld, m_rayToWorld, rayResult.m_hitFraction);
			return 1.f;
		}
	};


	AllRayResultCallback	resultCallback(rayFrom, rayTo);
	//	btCollisionWorld::ClosestRayResultCallback resultCallback(rayFrom,rayTo);
	world->rayTest(rayFrom, rayTo, resultCallback);
	if (resultCallback.hasHit())
	{
		hitNormal = Vector(resultCallback.m_hitNormalWorld[0], resultCallback.m_hitNormalWorld[2], resultCallback.m_hitNormalWorld[1]);
		hitLocation = Vector(resultCallback.m_hitPointWorld[0], resultCallback.m_hitPointWorld[2], resultCallback.m_hitPointWorld[1]);
		return true;
	}
	return false;
}
