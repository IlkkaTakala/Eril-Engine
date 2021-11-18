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

void ParseChildren(rapidxml::xml_node<>* node) 
{
	for (auto n = node->first_node(); n; n = n->next_sibling())
	{
		Console::Log(node->name() + String("> found node: ") + n->name());
		String type = n->name();
		if (ObjectManager::TypeList.find(type) == ObjectManager::TypeList.end()) continue;

		uint32 id = 0;
		String args;
		auto id_ptr = n->first_attribute("id");
		if (id_ptr != 0) id = std::stoul(id_ptr->value(), nullptr, 16);
		auto arg_ptr = n->first_attribute("args");
		if (arg_ptr != 0) args = arg_ptr->value();

		BaseObject* obj = ObjectManager::TypeList[type](args, id, Constants::Record::LOADED, false, 0);
		Console::Log("Spawned object with record: " + obj->GetRecord().ToString());

		ParseChildren(n);
	}
}

void Scene::OpenLevel(String map)
{
	using namespace rapidxml;
	Loop->World = SpawnObject<Scene>();

	String loaded;
	FileManager::RequestData(map + ".map", loaded);

	xml_document<>* doc = new xml_document<>();
	doc->parse<0>(loaded.data());

	/*Console::Log("Name of my first node is: " + String(doc.first_node()->name()));
	xml_node<>* node = doc.first_node("foobar");
	Console::Log("Node foobar has value " + String(node->value()));
	for (xml_attribute<>* attr = node->first_attribute();
		attr; attr = attr->next_attribute())
	{
		Console::Log("Node foobar has attribute " + String(attr->name()));
		Console::Log("with value " + String(attr->value()));
	}*/

	ParseChildren(doc);

	delete doc;
}

void Scene::AddSceneRoot(SceneComponent* obj)
{
	SceneGraph.push_back(obj);
}

void Scene::RemoveSceneRoot(SceneComponent* obj)
{
	SceneGraph.remove(obj);
}
