#pragma once
#include "btBulletDynamicsCommon.h"
#include "SceneComponent.h"

class Actor;
class MovementComponent;
class btPairCachingGhostObject;
class ErilMotion;

class ColliderComponent : public SceneComponent, public Tickable
{
    REGISTER(ColliderComponent);
public:
    ColliderComponent();
    virtual void OnDestroyed() override;
    virtual void LoadWithParameters(const String& args) override;
    virtual void Tick(float delta) override;
    virtual void BeginPlay() override {}
    virtual void SetType(int t);
    void SetMass(float m);
    void SetTarget(SceneComponent* m);
    void SetMovementTarget(MovementComponent* m);
    MovementComponent* GetMovementTarget() const { return moveObject; }
    SceneComponent* GetTarget() const;
    int GetType() const { return type; }
    virtual void SetParent(SceneComponent* parent) override;
    virtual void Refresh() override;

    void OnCollide();
protected:
    friend class ErilMotion;
    btRigidBody* body;
    int type;
    AABB size;
    float mass;
    Ref<SceneComponent> Object;
    Ref<MovementComponent> moveObject;
};

class ErilMotion : public btMotionState
{
    btTransform m_centerOfMassOffset;
    RefWeak<ColliderComponent> m_userPointer;
public:

    ErilMotion(ColliderComponent* c, const btTransform& startTrans = btTransform::getIdentity(), const btTransform& centerOfMassOffset = btTransform::getIdentity()) :
        m_centerOfMassOffset(centerOfMassOffset),
        m_userPointer(c)
    {
    }

    ///synchronizes world transform from user to physics
    virtual void getWorldTransform(btTransform& centerOfMassWorldTrans) const;
    virtual void setWorldTransform(const btTransform& centerOfMassWorldTrans);
};