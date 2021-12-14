#include "VisibleObject.h"
#include "Physics.h"
#include <Gameplay/Scene.h>

SceneComponent::SceneComponent() : BaseObject()
{
	Location = Vector(0, 0, 0);
	Rotation = Vector(0, 0, 0);
	Scale = Vector(1, 1, 1);

	transformForce = false;

	Parent = nullptr;
	GetScene()->AddSceneRoot(this);
}

void SceneComponent::OnDestroyed()
{
	if (Parent != nullptr) Parent->RemoveComponent(this);
	else GetScene()->RemoveSceneRoot(this);
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

	if (loc != data.end()) SetLocation(Vector(loc->second), true);
	if (rot != data.end()) SetRotation(Vector(rot->second), true);
	if (scale != data.end()) SetScale(Vector(scale->second), true);
}

void SceneComponent::SetLocation(const Vector& NewLocation, bool force)
{
	Location = NewLocation;
	desired.Location = NewLocation;
	transformForce = force;
}

void SceneComponent::SetRotation(const Vector& NewRotation, bool force)
{
	Rotation = NewRotation;
	desired.Rotation = NewRotation;
	transformForce = force;
}

void SceneComponent::SetScale(const Vector& NewScale, bool force)
{
	Scale = NewScale;
	desired.Scale = NewScale;
	transformForce = force;
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
