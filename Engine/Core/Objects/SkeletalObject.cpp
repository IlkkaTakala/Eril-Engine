#include "SkeletalObject.h"
#include "Physics.h"
#include "Mesh.h"

SkeletalObject::SkeletalObject() : SceneComponent()
{
	RenderData = nullptr;
	animControl = nullptr;
	//Physics::AddStatic(this);
}

void SkeletalObject::OnDestroyed()
{
	if (RenderData) RenderData->SetVisible(false);
	SceneComponent::OnDestroyed();
	//Physics::RemoveStatic(this);
}

void SkeletalObject::LoadWithParameters(const String& args)
{
	SceneComponent::LoadWithParameters(args);
	auto data = ParseOptions(args);

	auto mat1 = data.find("Material1");
	auto mat2 = data.find("Material2");
	auto mat3 = data.find("Material3");
	auto mesh = data.find("Mesh");

	if (mesh != data.end()) SetModel(mesh->second);
	if (mat1 != data.end()) GetModel()->SetMaterial(0, IRender::LoadMaterialByName(mat1->second));
	if (mat2 != data.end()) GetModel()->SetMaterial(1, IRender::LoadMaterialByName(mat2->second));
	if (mat3 != data.end()) GetModel()->SetMaterial(2, IRender::LoadMaterialByName(mat3->second));
}

void SkeletalObject::SetModel(std::string Name)
{
	if (RenderData) delete RenderData;
	RenderData = MI->GetSkeletal(this, Name);
	if (RenderData) RenderData->AddRenderCallback("animation", std::bind(&SkeletalObject::UpdateAnimations, this, std::placeholders::_1));
}

void SkeletalObject::SetModelAsync(std::string Name)
{
	if (RenderData) delete RenderData;
	RenderData = MI->GetSkeletalAsync(this, Name);
	if (RenderData) RenderData->AddRenderCallback("animation", std::bind(&SkeletalObject::UpdateAnimations, this, std::placeholders::_1));
}

void SkeletalObject::SetModel(RenderMesh* mesh)
{
	if (RenderData) delete RenderData;
	if (mesh && mesh->GetMeshType() == RenderMesh::MeshType::Skeletal) {
		RenderData = mesh;
		RenderData->SetParent(this);
		RenderData->AddRenderCallback("animation", std::bind(&SkeletalObject::UpdateAnimations, this, std::placeholders::_1));
	}
}

void SkeletalObject::SetAnimController(AnimationController* a)
{
	animControl = a;
	RenderData->AddRenderCallback("animation", std::bind(&SkeletalObject::UpdateAnimations, this, std::placeholders::_1));
}

void SkeletalObject::UpdateAnimations(float delta)
{
	if (animControl) animControl->UpdateBoneTransforms(delta, RenderData);
}
