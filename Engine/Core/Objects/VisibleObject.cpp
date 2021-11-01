#include "VisibleObject.h"
#include "Physics.h"

VisibleObject::VisibleObject() : SceneComponent()
{
	RenderData = nullptr;
	Physics::AddStatic(this);
}

void VisibleObject::OnDestroyed()
{
	Physics::RemoveStatic(this);
}

void VisibleObject::SetModel(std::string Name)
{
	RenderData = MI->LoadData(this, Name);
	if (RenderData != nullptr) RenderData->ApplyTransform();
}

void VisibleObject::SetModel(RenderMesh* mesh)
{
	RenderData = mesh;
	if (RenderData != nullptr) RenderData->ApplyTransform();
}
