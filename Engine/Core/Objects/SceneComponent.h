#pragma once
#include <Core.h>

class MovementComponent;

class SceneComponent : public BaseObject
{
public:
	SceneComponent();
	virtual ~SceneComponent() { }
	virtual void BeginPlay() {}
	virtual void OnDestroyed() override;
	virtual void LoadWithParameters(const String& args) override;

	virtual void SetLocation(const Vector& NewLocation, bool force = false);
	virtual void SetRotation(const Vector& NewRotation, bool force = false);
	void SetScale(const Vector& NewScale, bool force = false);
	void AddLocation(const Vector& NewLocation) { SetLocation(NewLocation + Location); };
	void AddRotation(const Vector& NewRotation) { SetRotation(NewRotation + Rotation); };
	const Vector GetLocation() const { return Location; }
	const Vector GetRotation() const { return Rotation; }
	const Vector GetWorldLocation() const { return (Parent ? Parent->GetWorldLocation() : Vector(0.f)) + Location; }
	const Vector GetWorldRotation() const { return Rotation; }
	const Vector GetScale() const { return Scale; }
	Transformation GetTransformation() const { return Transformation(Location, Rotation, Scale); }

	SceneComponent* GetParent() const { return Parent; }
	const std::list<RefWeak<SceneComponent>> GetChildren() const { return Children; }
	//void SetParent(SceneComponent* obj) { Parent = obj; }

	void AddComponent(SceneComponent* obj);
	void RemoveComponent(SceneComponent* obj);

protected:
	friend class MovementComponent;

	Ref<SceneComponent> Parent;
	std::list<RefWeak<SceneComponent>> Children;

	Vector Location;
	Vector Rotation;
	Vector Scale;

	bool transformForce;
	Transformation desired;
};

