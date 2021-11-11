#include "Scene.h"
#include "Objects/SceneComponent.h"
#include "GameLoop.h"

Scene::Scene() : BaseObject()
{
	gravity = 10.f;
	SceneGraph.clear();
}

void Scene::OpenLevel(String map)
{
	Loop->World = SpawnObject<Scene>();


}

void Scene::AddSceneRoot(SceneComponent* obj)
{
	SceneGraph.push_back(obj);
}

void Scene::RemoveSceneRoot(SceneComponent* obj)
{
	SceneGraph.remove(obj);
}
