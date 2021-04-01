#pragma once
#include "Core.h"
#include <string>
#include "Mesh.h"
#include "Material.h"

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
	void SetMaterial(Material* nextMat) { MaterialInstance = nextMat; }

	const std::string GetName() const { return std::string(""); }

	virtual void BeginPlay() override {};
protected:
	friend class GC;
	
	Material* MaterialInstance;
	MeshData* Model;

	Vector Location;
	Vector Rotation;
	Vector Scale;
};