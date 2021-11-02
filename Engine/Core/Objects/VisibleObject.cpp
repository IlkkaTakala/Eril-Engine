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
}

void VisibleObject::SetModel(RenderMesh* mesh)
{
	RenderData = mesh;
}
