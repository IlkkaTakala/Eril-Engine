#pragma once
#include <Core.h>

class SceneComponent;

class Scene : public BaseObject
{
public:
	Scene();

	virtual void BeginPlay() {}

	static void OpenLevel(String map);
	static void CheckShouldLoad();

	void OnDestroyed() override;
	void AddSceneRoot(SceneComponent*);
	void RemoveSceneRoot(SceneComponent*);

private:
	static void LoadLevel();

	static String newLevel;

	std::list<Ref<SceneComponent>> SceneGraph;

	float gravity;
};