#pragma once
#include "btBulletDynamicsCommon.h"
#include "SceneComponent.h"

class MovementComponent;

class ColliderComponent : public SceneComponent, public Tickable
{
    REGISTER(ColliderComponent);
public:
    ColliderComponent();
    virtual void OnDestroyed() override;
    virtual void LoadWithParameters(const String& args) override;
    virtual void Tick(float) override;
    void SetType(int t);
    void SetSize(AABB s);
    void SetMass(float m);
    void SetTarget(MovementComponent* m);
    virtual void SetLocation(const Vector& NewLocation, bool force = false) override;
    virtual void SetRotation(const Vector& NewRotation, bool force = false) override;
    void ApplyCollision();
    int GetType() const { return type; }

private:
    btRigidBody* body;
    int type;
    AABB size;
    float mass;
    Ref<MovementComponent> Object;
};
