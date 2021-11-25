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

void VisibleObject::LoadWithParameters(const String& args)
{
	SceneComponent::LoadWithParameters(args);
	auto data = ParseOptions(args);

	auto mat = data.find("Material");
	auto mesh = data.find("Mesh");

	if (mesh != data.end()) SetModel(mesh->second);
	if (mat != data.end()) GetModel()->SetMaterial(atoi(&mat->second[0]), RI->LoadMaterialByName(&mat->second[1]));
}

void VisibleObject::SetModel(std::string Name)
{
	RenderData = MI->LoadData(this, Name);
}

void VisibleObject::SetModel(RenderMesh* mesh)
{
	RenderData = mesh;
}
