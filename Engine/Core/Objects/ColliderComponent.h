#pragma once
#include "btBulletDynamicsCommon.h"
#include "SceneComponent.h"

class Actor;
class MovementComponent;
class ErilMotion;

class ColliderComponent : public SceneComponent, public Tickable
{
    REGISTER(ColliderComponent);
public:
    ColliderComponent();
    virtual void OnDestroyed() override;
    virtual void LoadWithParameters(const String& args) override;
    void SetType(int t);
    void SetSize(AABB s);
    void SetMass(float m);
    void SetTarget(SceneComponent* m);
    void SetMovementTarget(MovementComponent* m);
    MovementComponent* GetMovementTarget() const { return moveObject; }
    SceneComponent* GetTarget() const;
    int GetType() const { return type; }
    virtual void SetParent(SceneComponent* parent) override;
    virtual void Refresh() override;

private:
    friend class ErilMotion;
    btRigidBody* body;
    int type;
    AABB size;
    float mass;
    Ref<SceneComponent> Object;
    Ref<MovementComponent> moveObject;
};
