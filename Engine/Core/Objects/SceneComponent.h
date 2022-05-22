#pragma once
#include <Core.h>

class MovementComponent;

class SceneComponent : public BaseObject
{
public:
	SceneComponent();
	virtual ~SceneComponent() { }
	virtual void BeginPlay();
	virtual void OnDestroyed() override;
	virtual void LoadWithParameters(const String& args) override;

	virtual void SetLocation(const Vector& NewLocation, bool force = false);
	virtual void SetRotation(const Rotator& NewRotation, bool force = false);
	virtual void SetWorldLocation(const Vector& NewLocation, bool force = false);
	virtual void SetWorldRotation(const Rotator& NewRotation, bool force = false);
	void SetScale(const Vector& NewScale, bool force = false);
	void SetWorldScale(const Vector& NewScale, bool force = false);
	void AddLocation(const Vector& NewLocation) { SetLocation(NewLocation + Location); };
	void AddRotation(const Rotator& NewRotation) { SetRotation(NewRotation + Rotation); };
	const Vector GetLocation() const { return Location; }
	const Rotator GetRotation() const { return Rotation; }
	const Vector GetWorldLocation() const { return (Parent ? Parent->GetWorldLocation() : Vector(0.f)) + (Parent ? Parent->GetWorldRotation() : 1.f) * Location; }
	const Rotator GetWorldRotation() const { return (Parent ? Parent->GetWorldRotation() : Rotator(0.f)) * Rotation; }
	const Vector GetScale() const { return Scale; }
	const Vector GetWorldScale() const { return Parent ? Parent->GetWorldScale() : Vector(0.f) * Scale; }
	Transformation GetTransformation() const { return Transformation(Location, Rotation, Scale); }
	Transformation GetWorldTransformation() const { return Transformation(GetWorldLocation(), GetWorldRotation(), GetWorldScale()); }
	void SetWorldTransform(const Transform& world);
	void SetTransform(const Transform& world) { Location = world.Location; Rotation = world.Rotation; Scale = world.Scale; }

	SceneComponent* GetParent() const { return Parent; }
	virtual void SetParent(SceneComponent* parent) { Parent = parent; }
	const std::list<Ref<SceneComponent>>& GetChildren() const { return Children; }
	//void SetParent(SceneComponent* obj) { Parent = obj; }

	virtual void Refresh() {}

	void AddComponent(SceneComponent* obj);
	void RemoveComponent(SceneComponent* obj);

protected:
	friend class MovementComponent;

	Ref<SceneComponent> Parent;
	std::list<Ref<SceneComponent>> Children;

	Vector Location;
	Rotator Rotation;
	Vector Scale;

	bool transformForce;
	Transformation desired;
};

