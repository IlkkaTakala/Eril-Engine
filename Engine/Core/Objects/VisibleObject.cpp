#include "VisibleObject.h"
#include "Physics.h"
#include "Mesh.h"
#ifdef USE_SCRIPTCORE
#include <ScriptCore.h>
namespace ScriptFunctions {
	int64 SetModel(int64 id, String* name) {
		auto m = ObjectManager::GetByRecord<BaseObject>(id);
		auto model = dynamic_cast<VisibleObject*>(m);
		if (model) {
			model->SetModel(*name);
			Console::Log("Model Changed to: " + *name);
		}
		else {
			Console::Log("Can't change model");
		}
		return id;
	}

	int64 SetMaterial(int64 id, String* name) {
		auto m = ObjectManager::GetByRecord<BaseObject>(id);
		auto mater = dynamic_cast<VisibleObject*>(m);
		if (mater) {
			mater->GetModel()->SetMaterial(0, IRender::LoadMaterialByName("Assets/Materials/" + *name));
			Console::Log("Material Changed to: " + *name);
		}
		else {
			Console::Log("Can't change material");
		}
		return id;
	}
	REGISTER_FUNCTION(SetModel, global, 2);
	REGISTER_FUNCTION(SetMaterial, global, 2);

}
#endif

VisibleObject::VisibleObject() : SceneComponent()
{
	RenderData = nullptr;
	Physics::AddStatic(this);
}

void VisibleObject::OnDestroyed()
{
	if (RenderData) RenderData->SetVisible(false);
	SceneComponent::OnDestroyed();
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
	if (mat1 != data.end()) GetModel()->SetMaterial(0, IRender::LoadMaterialByName(mat1->second));
	if (mat2 != data.end()) GetModel()->SetMaterial(1, IRender::LoadMaterialByName(mat2->second));
	if (mat3 != data.end()) GetModel()->SetMaterial(2, IRender::LoadMaterialByName(mat3->second));
}

void VisibleObject::SetModel(std::string Name)
{
	delete RenderData;
	RenderData = MI->GetStatic(this, Name);
}

void VisibleObject::SetModel(RenderMesh* mesh)
{
	if (mesh && mesh->GetMeshType() == RenderMesh::MeshType::Static) {
		delete RenderData;
		RenderData = mesh;
	}
}

