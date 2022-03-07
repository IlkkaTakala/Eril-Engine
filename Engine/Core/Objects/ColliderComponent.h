#pragma once
#include "btBulletDynamicsCommon.h"
#include "SceneComponent.h"

class Actor;
class MovementComponent;
class ErilMotion;
class btPairCachingGhostObject;

class ColliderComponent : public SceneComponent, public Tickable
{
    REGISTER(ColliderComponent);
public:
    ColliderComponent();
    virtual void OnDestroyed() override;
    virtual void LoadWithParameters(const String& args) override;
    virtual void Tick(float delta) override;
    virtual void BeginPlay() override {}
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

    void OnCollide();
private:
    friend class ErilMotion;
    btRigidBody* body;
    int type;
    AABB size;
    float mass;
    Ref<SceneComponent> Object;
    Ref<MovementComponent> moveObject;
};
