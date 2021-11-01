#pragma once
#include <Core.h>

class SceneComponent : public BaseObject
{
public:
	SceneComponent();
	virtual ~SceneComponent() { }

	void SetLocation(Vector NewLocation);
	void SetRotation(Vector NewRotation);
	void SetScale(Vector NewScale);
	void AddLocation(Vector NewLocation) { SetLocation(NewLocation + Location); };
	void AddRotation(Vector NewRotation) { SetRotation(NewRotation + Rotation); };
	const Vector GetLocation() const { return Location; }
	const Vector GetRotation() const { return Rotation; }
	const Vector GetScale() const { return Scale; }

protected:

	Vector Location;
	Vector Rotation;
	Vector Scale;
};

