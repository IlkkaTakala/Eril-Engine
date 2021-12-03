#include "VisibleObject.h"
#include "Physics.h"
#include "Mesh.h"

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

	auto mat1 = data.find("Material1");
	auto mat2 = data.find("Material2");
	auto mat3 = data.find("Material3");
	auto mesh = data.find("Mesh");

	if (mesh != data.end()) SetModel(mesh->second);
	if (mat1 != data.end()) GetModel()->SetMaterial(0, RI->LoadMaterialByName(mat1->second));
	if (mat2 != data.end()) GetModel()->SetMaterial(1, RI->LoadMaterialByName(mat2->second));
	if (mat3 != data.end()) GetModel()->SetMaterial(2, RI->LoadMaterialByName(mat3->second));
}

void VisibleObject::SetModel(std::string Name)
{
	RenderData = MI->LoadData(this, Name);
}

void VisibleObject::SetModel(RenderMesh* mesh)
{
	RenderData = mesh;
}
