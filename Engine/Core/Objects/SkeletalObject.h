#pragma once
#include <Core.h>
#include "SceneComponent.h"
#include "AnimationController.h"

class SkeletalObject : public SceneComponent
{
	REGISTER(SkeletalObject);
public:
	SkeletalObject();
	virtual ~SkeletalObject() { delete RenderData; }
	virtual void OnDestroyed() override;
	virtual void LoadWithParameters(const String& args) override;

	void SetModel(std::string Name);
	void SetModelAsync(std::string Name);
	void SetModel(RenderMesh* mesh);
	std::string GetModelName() const { return std::string(); }
	RenderMesh* GetModel() const { return RenderData; }

	void SetAnimController(AnimationController* a);
	const AnimationController* GetAnimController() const { return animControl; }

	const std::string GetName() const { return std::string(""); }

	virtual void BeginPlay() override {};
protected:
	friend class GC;

	Ref<AnimationController> animControl;
	RenderMesh* RenderData;

	void UpdateAnimations(float delta);
};

