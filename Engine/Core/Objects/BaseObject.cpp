#include "BaseObject.h"
#include "GarbageCollector.h"
#include "ObjectManager.h"
#include <GameLoop.h>
#include <locale>
#include <Gameplay/Scene.h>

//void BaseObject::operator delete(void* ptr)
//{ 
//	//GC::RemoveObject(reinterpret_cast<BaseObject*>(ptr));
//	::delete ptr;
//}

void Data::DestroyObject()
{
	OnDestroyed();
	ObjectManager::DeleteRecord(RecordNumber);
}

Data::Data()
{
	RecordNumber = 0;
	hasLife = false;
	lifetime = 0.f;
	bMarked = false;
	ObjectManager::CreateRecord(this);
}

Data::~Data()
{

}

void Data::AddToRoot()
{
	ObjectManager::Protect(RecordNumber);
}

void Data::RemoveFromRoot()
{
	ObjectManager::Unprotect(RecordNumber);
}

void Data::SetLifetime(float in)
{
	lifetime = in;
	hasLife = !(in - 0.000001f < 0.f && in + 0.000001f > 0.f);
}

float Data::GetRemainingLifetime()
{
	return lifetime;
}

BaseObject::BaseObject()
{
	active = false;
	World = Loop->World;
}

void BaseObject::RegisterInputs(InputComponent* IC)
{

}

void BaseObject::SetColliders(ColliderComponent* CC)
{

}

void BaseObject::SetScene(Scene* scene)
{
	if (World) World->RemoveObject(this);
	if (scene) scene->AddObject(this); 
	World = scene;
}

const std::map<String, String> BaseObject::ParseOptions(const String& args)
{
	std::map<String, String> options;
	String temp = args;
	temp.erase(std::remove_if(temp.begin(), temp.end(),
		[](char& c) {
			return std::isspace<char>(c, std::locale::classic());
		}), temp.end());
	auto out = split(temp, ';');

	for (const auto& s : out) {
		auto d = split(s, ':');
		if (d.size() == 2) {
			options.emplace(d[0], d[1]);
		}
	}

	return options;
}
