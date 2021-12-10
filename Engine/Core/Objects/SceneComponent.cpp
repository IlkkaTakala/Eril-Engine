#include "VisibleObject.h"
#include "Physics.h"
#include <Gameplay/Scene.h>

SceneComponent::SceneComponent() : BaseObject()
{
	Location = Vector(0, 0, 0);
	Rotation = Vector(0, 0, 0);
	Scale = Vector(1, 1, 1);

	Parent = nullptr;
	GetScene()->AddSceneRoot(this);
}

void SceneComponent::OnDestroyed()
{
	if (Parent != nullptr) Parent->RemoveComponent(this);
	for (const auto& c : Children) {
		c->Parent = nullptr;
		c->DestroyObject();
	}
}

void SceneComponent::LoadWithParameters(const String& args)
{
	BaseObject::LoadWithParameters(args);
	auto data = ParseOptions(args);

	auto loc = data.find("Location");
	auto rot = data.find("Rotation");
	auto scale = data.find("Scale");

	if (loc != data.end()) SetLocation(Vector(loc->second));
	if (rot != data.end()) SetRotation(Vector(rot->second));
	if (scale != data.end()) SetScale(Vector(scale->second));
}

void SceneComponent::SetLocation(const Vector& NewLocation)
{
	Location = NewLocation;
}

void SceneComponent::SetRotation(const Vector& NewRotation)
{
	Rotation = NewRotation;
}

void SceneComponent::SetScale(const Vector& NewScale)
{
	Scale = NewScale;
}

void SceneComponent::AddComponent(SceneComponent* obj)
{
	GetScene()->RemoveSceneRoot(obj);
	Children.push_back(obj);
	obj->Parent = this;
}

void SceneComponent::RemoveComponent(SceneComponent* obj)
{
	Children.remove(obj);
}
