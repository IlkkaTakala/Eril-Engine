#include "Scene.h"
#include "Objects/SceneComponent.h"
#include "GameLoop.h"
#include <Interface/FileManager.h>
#include <RapidXML.hpp>

Scene::Scene() : BaseObject()
{
	gravity = 10.f;
	SceneGraph.clear();
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
		c->BeginPlay();
		auto t = dynamic_cast<Tickable*>(base.GetPointer());
		ObjectManager::AddTick(t);

		LoopSceneChildren(c);
	}
}

void Scene::OpenLevel(String map)
{
	if (Loop->World != nullptr) ObjectManager::CleanObjects();
	using namespace rapidxml;
	Loop->World = SpawnObject<Scene>();

	String loaded;
	FileManager::RequestData(map + ".map", loaded);

	xml_document<>* doc = new xml_document<>();
	doc->parse<0>(loaded.data());

	ParseChildren(doc, nullptr);

	delete doc;

	for (const auto& base : Loop->World->SceneGraph) {
		base->BeginPlay();
		auto t = dynamic_cast<Tickable*>(base.GetPointer());
		ObjectManager::AddTick(t);

		LoopSceneChildren(base);
	}
	
}

void Scene::OnDestroyed()
{
	for (const auto& c : SceneGraph) {
		c->DestroyObject();
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
