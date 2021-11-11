#pragma once
#include <Core.h>
#include <Objects/SceneComponent.h>
#include "Interface/IRender.h"

class VisibleObject : public SceneComponent
{
public:
	VisibleObject();
	virtual ~VisibleObject() { delete RenderData; }
	virtual void OnDestroyed() override;

	void SetModel(std::string Name);
	void SetModel(RenderMesh* mesh);
	std::string GetModelName() const { return std::string(); }
	RenderMesh* GetModel() const { return RenderData; }

	const std::string GetName() const { return std::string(""); }

	virtual void BeginPlay() override {};
protected:
	friend class GC;

	RenderMesh* RenderData;
	
};