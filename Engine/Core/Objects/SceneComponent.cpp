#include "SceneComponent.h"
#include "Physics.h"
#include <Gameplay/Scene.h>
#ifdef USE_SCRIPTCORE
#include <ScriptCore.h>
namespace ScriptFunctions {
	int64 SetLocation(int64 id, int x, int y, int z) {
		auto l = ObjectManager::GetByRecord<BaseObject>(id);
		auto scene = dynamic_cast<SceneComponent*>(l);
		if (scene) {
			scene->SetLocation(Vector( x, y, z ));
			Console::Log("Location changed to: " + scene->GetLocation().ToString());
		}
		else {
			Console::Log("Can't change location");
		}
		return id;
	}

	int64 SetRotation(int64 id, int x, int y, int z) {
		auto r = ObjectManager::GetByRecord<BaseObject>(id);
		auto scene = dynamic_cast<SceneComponent*>(r);
		if (scene) {
			scene->SetRotation(Vector(x, y, z));
			Console::Log("Rotation changed to: ");
		}
		else {
			Console::Log("Can't change rotation");
		}
		return id;
	}

	int64 SetScale(int64 id, int x, int y, int z) {
		auto s = ObjectManager::GetByRecord<BaseObject>(id);
		auto scene = dynamic_cast<SceneComponent*>(s);
		if (scene) {
			scene->SetScale(Vector(x, y, z));
			Console::Log("Scale changed to: " + scene->GetScale().ToString());
		}
		else {
			Console::Log("Can't change scale");
		}
		return id;
	}

	int64 DestroyObject(int64 id) {
		auto s = ObjectManager::GetByRecord<BaseObject>(id);
		auto scene = dynamic_cast<SceneComponent*>(s);
		if (scene) {
			scene->DestroyObject();
			Console::Log("Destroyed object: " + scene->GetRecord().ToString());
		} else {
			Console::Log("Can't destroy object");
		}
		return id;
	}

	REGISTER_FUNCTION(SetLocation, global, 4);
	REGISTER_FUNCTION(SetRotation, global, 4);
	REGISTER_FUNCTION(SetScale, global, 4);
	REGISTER_FUNCTION(DestroyObject, global, 1);
}
#endif // SCRIPTCORE


SceneComponent::SceneComponent() : BaseObject()
{
	Location = Vector(0, 0, 0);
	Rotation = Rotator(0.f);
	Scale = Vector(1, 1, 1);

	transformForce = false;

	Parent = nullptr;
	GetScene()->AddSceneRoot(this);
}

void SceneComponent::OnDestroyed()
{
	if (Parent != nullptr) Parent->RemoveComponent(this);
	else if (GetScene()) GetScene()->RemoveSceneRoot(this);
	for (const auto& c : Children) {
		if (!c) continue;
		c->Parent = nullptr;
		c->DestroyObject();
	}
	Children.clear();
}

void SceneComponent::LoadWithParameters(const String& args)
{
	BaseObject::LoadWithParameters(args);
	auto data = ParseOptions(args);

	auto loc = data.find("Location");
	auto rot = data.find("Rotation");
	auto scale = data.find("Scale");

	if (loc != data.end()) SetLocation(Vector(loc->second), true);
	if (rot != data.end()) SetRotation(Rotator(Vector(rot->second)), true);
	if (scale != data.end()) SetScale(Vector(scale->second), true);
}

void SceneComponent::SetLocation(const Vector& NewLocation, bool force)
{
	Location = NewLocation;
	desired.Location = NewLocation;
	transformForce = force;
	for (auto& c : Children) c->Refresh();
}

void SceneComponent::SetRotation(const Rotator& NewRotation, bool force)
{
	Rotation = NewRotation;
	desired.Rotation = NewRotation;
	transformForce = force;
	if (force) for (auto& c : Children) c->Refresh();
}

void SceneComponent::SetWorldLocation(const Vector& NewLocation, bool force)
{
	Location = NewLocation - Parent->GetWorldLocation();
	if (force) for (auto& c : Children) c->Refresh();
}

void SceneComponent::SetWorldRotation(const Rotator& NewRotation, bool force)
{
	Rotation = NewRotation - Parent->GetWorldRotation();
	if (force) for (auto& c : Children) c->Refresh();
}

void SceneComponent::SetScale(const Vector& NewScale, bool force)
{
	Scale = NewScale;
	desired.Scale = NewScale;
	transformForce = force;
	if (force) for (auto& c : Children) c->Refresh();
}

void SceneComponent::SetWorldScale(const Vector& NewScale, bool force)
{
	Scale = NewScale / Parent->GetWorldScale();
	if (force) for (auto& c : Children) c->Refresh();
}

void SceneComponent::SetWorldTransform(const Transform& world)
{
	SetWorldLocation(world.Location); 
	SetWorldRotation(world.Rotation); 
	SetWorldScale(world.Scale);
	for (auto& c : Children) c->Refresh();
}

void SceneComponent::AddComponent(SceneComponent* obj)
{
	GetScene()->RemoveSceneRoot(obj);
	Children.push_back(obj);
	obj->SetParent(this);
	obj->Refresh();
}

void SceneComponent::RemoveComponent(SceneComponent* obj)
{
	Children.remove(obj);
}
