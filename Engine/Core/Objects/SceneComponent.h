#pragma once
#include <Core.h>

class SceneComponent : public BaseObject
{
public:
	SceneComponent();
	virtual ~SceneComponent() { }
	virtual void BeginPlay() {}
	virtual void OnDestroyed() override;

	void SetLocation(const Vector& NewLocation);
	void SetRotation(const Vector& NewRotation);
	void SetScale(const Vector& NewScale);
	void AddLocation(const Vector& NewLocation) { SetLocation(NewLocation + Location); };
	void AddRotation(const Vector& NewRotation) { SetRotation(NewRotation + Rotation); };
	const Vector GetLocation() const { return Location; }
	const Vector GetRotation() const { return Rotation; }
	const Vector GetScale() const { return Scale; }
	Transformation GetTransformation() const { return Transformation(Location, Rotation, Scale); }

	SceneComponent* GetParent() const { return Parent; }
	//void SetParent(SceneComponent* obj) { Parent = obj; }

	void AddComponent(SceneComponent* obj);
	void RemoveComponent(SceneComponent* obj);

protected:

	SceneComponent* Parent;
	std::list<RefWeak<SceneComponent>> Children;

	Vector Location;
	Vector Rotation;
	Vector Scale;
};

