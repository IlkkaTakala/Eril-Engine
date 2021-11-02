#pragma once
#include <Core.h>

class SceneComponent : public BaseObject
{
public:
	SceneComponent();
	virtual ~SceneComponent() { }

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
	void SetParent(SceneComponent* obj) { Parent = obj; }
protected:

	SceneComponent* Parent;

	Vector Location;
	Vector Rotation;
	Vector Scale;
};

