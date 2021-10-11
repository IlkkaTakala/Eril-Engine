#pragma once
#include "Core.h"
#include <string>
#include "IRender.h"

class VisibleObject : public BaseObject
{
public:
	VisibleObject();
	virtual ~VisibleObject() { delete RenderData; }
	virtual void OnDestroyed() override;

	void SetLocation(Vector NewLocation);
	void SetRotation(Vector NewRotation);
	void SetScale(Vector NewScale);
	void AddLocation(Vector NewLocation) { SetLocation(NewLocation + Location); };
	void AddRotation(Vector NewRotation) { SetRotation(NewRotation + Rotation); };
	const Vector GetLocation() const { return Location; }
	const Vector GetRotation() const { return Rotation; }
	const Vector GetScale() const { return Scale; }

	void SetModel(std::string Name);
	void SetModel(RenderMesh* mesh);
	std::string GetModelName() const { return std::string(); }
	RenderMesh* GetModel() const { return RenderData; }

	const std::string GetName() const { return std::string(""); }

	virtual void BeginPlay() override {};
protected:
	friend class GC;

	RenderMesh* RenderData;
	
	Vector Location;
	Vector Rotation;
	Vector Scale;
};