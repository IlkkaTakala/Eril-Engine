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
	static bool isLoading();
	static void SetLoadingMap(const String& name);

	void OnDestroyed() override;
	void AddSceneRoot(SceneComponent*);
	void RemoveSceneRoot(SceneComponent*);

	void AddObject(BaseObject* data);
	void RemoveObject(BaseObject* data);

private:
	static void LoadLevel();
	static void HardLoadLevel(const String& level);

	static String newLevel;
	static String loadMap;

	std::list<Ref<SceneComponent>> SceneGraph;
	std::list<RefWeak<BaseObject>> AllObjects;

	float gravity;
	bool bLoading;
	bool assetsLoading;
	std::unordered_map<String, bool> loadingStatus;
};