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

String Scene::newLevel = "";

Scene::Scene() : BaseObject()
{
	gravity = 10.f;
	SceneGraph.clear();
	bLoading = false;
}

void ParseChildren(rapidxml::xml_node<>* node, SceneComponent* base) 
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

			BaseObject* obj = ObjectManager::TypeList()[type](args, id, Constants::Record::LOADED, false, 0);
			Console::Log("Spawned object with record: " + obj->GetRecord().ToString());

			SceneComponent* scenic = dynamic_cast<SceneComponent*>(obj);
			if (scenic != nullptr && base != nullptr) base->AddComponent(scenic);

			obj->LoadWithParameters(args);
			ParseChildren(n, scenic);
		}
	}
}

void LoopSceneChildren(const Ref<SceneComponent>& base) 
{
	for (const auto& c : base->GetChildren()) {
		auto t = dynamic_cast<Tickable*>(base.GetPointer());
		ObjectManager::AddTick(t);
		c->BeginPlay();

		LoopSceneChildren(c);
	}
}

void Scene::OpenLevel(String map)
{
	newLevel = map;
}

void Scene::CheckShouldLoad()
{
	if (newLevel != "") LoadLevel();
}

bool Scene::isLoading()
{
	return Loop->World ? Loop->World->bLoading : false;
}

void Scene::OnDestroyed()
{
	while (SceneGraph.size() > 0) {
		if (*SceneGraph.begin() != nullptr) (*SceneGraph.begin())->DestroyObject();
		else SceneGraph.pop_front();
	}
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

void Scene::LoadLevel()
{
	WindowManager::SetShowCursor(0, false);
	if (Loop->World != nullptr) {
		ObjectManager::CleanObjects();
		II->ClearInputs();
		Physics::Destroy();
	}

	IECS::ResetECSWorld();

	using namespace rapidxml;

	ObjectManager::PrepareRecord(1, Constants::Record::LOADED);
	Loop->World = SpawnObject<Scene>();
	Loop->World->bLoading = true;
	Physics::init();

	ObjectManager::PrepareRecord(2, Constants::Record::LOADED);
	Ref<GameState> State = SpawnObject<GameState>();
	Loop->State = State;

	String loaded;
	FileManager::RequestData(newLevel + ".map", loaded);

	xml_document<>* doc = new xml_document<>();
	doc->parse<0>(loaded.data());

	ParseChildren(doc, nullptr);

	delete doc;

	Loop->World->BeginPlay();
	for (const auto& base : Loop->World->SceneGraph) {
		auto t = dynamic_cast<Tickable*>(base.GetPointer());
		ObjectManager::AddTick(t);
		base->BeginPlay();

		LoopSceneChildren(base);
	}

	RI->GetUIManager()->RegisterInputs();

	newLevel = "";
	Loop->World->bLoading = false;
}
