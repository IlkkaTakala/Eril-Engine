#pragma once
#include <Core.h>

class SceneComponent;

class Scene : public BaseObject
{
public:
	Scene();

	virtual void BeginPlay() {}

	static void OpenLevel(String map);

	void OnDestroyed() override;
	void AddSceneRoot(SceneComponent*);
	void RemoveSceneRoot(SceneComponent*);

private:

	std::list<Ref<SceneComponent>> SceneGraph;

	float gravity;
};