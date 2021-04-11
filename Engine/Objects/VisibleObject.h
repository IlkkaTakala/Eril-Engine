#pragma once
#include "Core.h"
#include <string>
#include "IRender.h"

class VisibleObject : public BaseObject
{
public:
	VisibleObject();
	virtual ~VisibleObject() { }

	void SetLocation(Vector NewLocation);
	void SetRotation(Vector NewRotation);
	void SetScale(Vector NewScale);
	void AddLocation(Vector NewLocation) { SetLocation(NewLocation + Location); };
	void AddRotation(Vector NewRotation) { SetRotation(NewRotation + Rotation); };

	void SetModel(std::string Name);
	std::string GetModel() const { return std::string(); }

	const std::string GetName() const { return std::string(""); }

	virtual void BeginPlay() override {};
protected:
	friend class GC;
	
	Vector Location;
	Vector Rotation;
	Vector Scale;
};