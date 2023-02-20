#include "Scene.h"
#include "Objects/SceneComponent.h"
#include "GameLoop.h"
#include <Interface/FileManager.h>
#include <RapidXML.hpp>
#include <Gameplay/GameState.h>
#include "RenderCore/OpenGL/UI/UISpace.h"
#include <Interface/WindowManager.h>
#include <Interface/IECS.h>
#include <Physics/BulletPhysics.h>
#include <Interface/AssetManager.h>
#include <algorithm>

String Scene::newLevel = "";
String Scene::loadMap = "";

std::map<String, Scene*> LoadedScenes;

Scene::Scene() : BaseObject()
{
	gravity = 10.f;
	SceneGraph.clear();
	bLoading = false;
	assetsLoading = false;
}

void ParseChildren(Scene* scene, rapidxml::xml_node<>* node, SceneComponent* base) 
{
	for (auto n = node->first_node(); n; n = n->next_sibling())
	{
		String type = n->name();
		if (ObjectManager::TypeList().find(type) == ObjectManager::TypeList().end()) continue;

		uint32 id = 0;
		String args;
		auto id_ptr = n->first_attribute("id");
		if (id_ptr != 0)
		{
			id = std::stoul(id_ptr->value(), nullptr, 16);
			auto arg_ptr = n->first_attribute("args");
			if (arg_ptr != 0) args = arg_ptr->value();

			BaseObject* obj = ObjectManager::TypeList()[type](scene, args, id, Constants::Record::LOADED, false, 0);
			obj->SetScene(scene);
			Console::Log("Spawned object with record: " + obj->GetRecord().ToString());

			SceneComponent* scenic = dynamic_cast<SceneComponent*>(obj);
			if (scenic != nullptr && base != nullptr) base->AddComponent(scenic);

			obj->LoadWithParameters(args);
			ParseChildren(scene, n, scenic);
		}
	}
}

void Scene::OpenLevel(String map)
{
	newLevel = map;
}

void Scene::CheckShouldLoad()
{
	if (Loop->World && Loop->World->assetsLoading) {
		auto& mp = Loop->World->loadingStatus;
		bool result = false;
		if (!mp.empty()) {
			result = std::all_of(mp.begin(), mp.end(),
				[](const auto& t) { return t.second; });
		}
		if (result) {
			HardLoadLevel(newLevel);
			newLevel = "";
		}
	}
	else if (newLevel != "") LoadLevel();
}

bool Scene::isLoading()
{
	return Loop->World ? Loop->World->bLoading : false;
}

void Scene::SetLoadingMap(const String& name)
{
	loadMap = name;
}

void Scene::OnDestroyed()
{
	while (AllObjects.size() > 0) {
		if (*AllObjects.begin() != nullptr) {
			auto ptr = *AllObjects.begin();
			ptr->SetScene(nullptr);
			ptr->DestroyObject();
		}
		else AllObjects.pop_front();
	}
	SceneGraph.clear();
	Loop->World = nullptr;
}

void Scene::AddSceneRoot(SceneComponent* obj)
{
	SceneGraph.push_back(obj);
}

void Scene::RemoveSceneRoot(SceneComponent* obj)
{
	SceneGraph.remove(obj);
}

void Scene::AddObject(BaseObject* data)
{
	AllObjects.push_back(data);

}

void Scene::RemoveObject(BaseObject* data)
{
	AllObjects.remove(data);
}

void Scene::HardLoadLevel(const String& level)
{
	if (Loop->World != nullptr) {
		ObjectManager::CleanObjects();
		II->ClearInputs();
		Physics::Destroy();
	}

	IECS::ResetECSWorld();

	using namespace rapidxml;

	ObjectManager::PrepareRecord(1, Constants::Record::LOADED);
	Loop->World = SpawnObject<Scene>(nullptr);
	Loop->World->bLoading = true;
	Physics::init();

	ObjectManager::PrepareRecord(2, Constants::Record::LOADED);
	Ref<GameState> State = SpawnObject<GameState>(nullptr);
	Loop->State = State;

	String loaded;
	FileManager::RequestData(level + ".map", loaded);

	xml_document<>* doc = new xml_document<>();
	doc->parse<0>(loaded.data());

	auto assets = doc->first_node("Asset");
	if (assets) {
		auto names = split(assets->value(), ',');
		for (auto& n : names) {
			AssetManager::LoadAsset(n.erase(0, n.find_first_not_of(" \t\n")).erase(n.find_last_not_of(" \t\n") + 1));
		}
	}

	ParseChildren(Loop->World, doc, nullptr);

	delete doc;

	Loop->World->BeginPlay();
	
	for (auto& c : Loop->World->AllObjects) {
		auto t = dynamic_cast<Tickable*>(c.GetPointer());
		ObjectManager::AddTick(t);
		c->BeginPlay();
		c->SetActive(true);
	}

	if (IRender::GetUIManager()) IRender::GetUIManager()->RegisterInputs();

	Loop->World->bLoading = false;
}

void Scene::LoadLevel()
{
	if (loadMap == "") HardLoadLevel(newLevel);
	else {
		HardLoadLevel(loadMap);

		String loaded;
		FileManager::RequestData(newLevel + ".map", loaded);
		using namespace rapidxml;
		xml_document<>* doc = new xml_document<>();
		doc->parse<0>(loaded.data());

		auto assets = doc->first_node("Asset");
		Loop->World->assetsLoading = true;
		if (assets) {
			auto names = split(assets->value(), ',');
			for (auto& n : names) {
				String as = n.erase(0, n.find_first_not_of(" \t\n")).erase(n.find_last_not_of(" \t\n") + 1);
				Loop->World->loadingStatus.emplace(as, false);
				AssetManager::LoadAssetAsync(as, [&](const String& name, bool) {
					Loop->World->loadingStatus[name] = true;
				});
			}
		}
	}
}
