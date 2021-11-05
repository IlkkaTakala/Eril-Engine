#pragma once
#include <Core.h>

class SceneComponent;

class Scene : public BaseObject
{
public:
	Scene();

	virtual void BeginPlay() {}

	static void OpenLevel(String map);

	void AddToRoot() {}
	void RemoveFromRoot() {}

private:

	std::list<Ref<SceneComponent>> SceneGraph;

	float gravity;
};