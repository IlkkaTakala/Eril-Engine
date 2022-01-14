#include "Editor.h"
#include <UI/Panel.h>
#include <UI/Text.h>
#include <UI/Button.h>
#include <UI/VerticalPanel.h>
#include <GamePlay/Scene.h>
#include "EditorItemWrapper.h"
#include <RenderCore/OpenGL/UI/UISpace.h>
#include <tinyfiledialogs.h>
#include <Interface/FileManager.h>
#include "Objects/InputComponent.h"

Editor::Editor()
{

}

Editor::~Editor()
{
}

void Editor::Construct()
{
	EditCanvas = Create<Panel>();
	EditCanvas->AddChild(
		Create<Image>()->SetStyle(Vector(0.5f))->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f, 1.f, 0.f))->SetHit(HitReg::HitTestInvisible)
	)->SetOrigin(0.5f, 0.5f)->SetTransform(0.f, 700.f, 0.f, 500.f, Vector(0.5f), Vector(0.5f));
	EditCanvas->hits = HitReg::HitTestVisible;
	EditCanvas->focusable = true;
	EditCanvas->SetEventCallback(Constants::UI::UI_ON_FOCUS, [this]() { SetActive(nullptr); });
	
	editing = nullptr; // EditCanvas;

	DataCanvas = Create<VerticalPanel>(); 
	DataCanvas->SetOrigin(0.f, 0.f)->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f, 1.f, 0.f));

	AddableComps = Create<VerticalPanel>();

	for (const auto& [name, spawn] : UI::UIList()) {
		AddableComps->AddChild(
			Create<Button>()->AddChild(
				Create<Text>()->SetText(name)->SetStyle(Vector(0.f))
			)->SetEventCallback(Constants::UI::UI_ON_MOUSE_UP, [this, name, spawn]() {
				auto button = dynamic_cast<Button*>(editing ? editing->item : EditCanvas);
				auto panel = dynamic_cast<Panel*>(editing ? editing->item : EditCanvas);
				if (!panel && !button) return;
				EditorItemWrapper* item = Create<EditorItemWrapper>();
				item->editer = this;
				item->item = spawn();
				item->item->SetTransform(0.f, 100.f, 0.f, 100.f, Vector(0.f), Vector(0.f));
				//item->item->SetParent(editing->item);
				if (panel) panel->AddChild(item->item);
				else if (button) button->AddChild(item->item);
				//this->Edit(this->editing, nullptr, name);
				item->z_index = 100;
				EditCanvas->AddChild(item);
				item->parent = item->item;
				SetActive(item);
				this->EditCanvas->UpdateMatrices(Vector2D());
			})->SetTransform(0.f, 0.f, 0.f, 40.f)
		);
	}

	TreeCanvas = Create<VerticalPanel>();

	AddComponent(
		Create<Panel>()
		->AddChild(
			EditCanvas
		)->AddChild(
			Create<Panel>()->AddChild(
				Create<Image>()->SetStyle(Vector(0.7f))->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f, 1.f, 0.f))
			)->AddChild(
				DataCanvas
			)->SetOrigin(1.f, 0.f)->SetTransform(0.f, 300.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(1.f))
		)->AddChild(
			Create<Panel>()->AddChild(
				Create<Image>()->SetStyle(Vector(0.2f))->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f, 1.f, 0.f))
			)->AddChild(
				AddableComps->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 0.5f, 0.f), Vector(0.f, 1.f, 0.f))
			)->AddChild(
				TreeCanvas->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.5f, 1.f, 0.f), Vector(0.f, 1.f, 0.f))
			)->SetTransform(0.f, 200.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f))
		)->AddChild(
			Create<Button>()->AddChild(
				Create<Text>()->SetText("Save XML", 20)->SetStyle(Vector(0.f))
			)->SetOrigin(0.5f, 0.f)->SetTransform(-200.f, 100.f, -50.f, 25.f, Vector(1.f), Vector(0.5f))
			->SetEventCallback(Constants::UI::UI_ON_MOUSE_UP, [this]() {
				char const* filters[1] = { "*.ui" };
				const char* dat = tinyfd_saveFileDialog("Save UI file", NULL, 1, filters, "XML UI (.ui)");
				SaveUI(dat ? dat : "");
			})
		)->AddChild(
			Create<Button>()->AddChild(
				Create<Text>()->SetText("Load XML", 20)->SetStyle(Vector(0.f))
			)->SetOrigin(0.5f, 0.f)->SetTransform(200.f, 100.f, -50.f, 25.f, Vector(1.f), Vector(0.5f))
			->SetEventCallback(Constants::UI::UI_ON_MOUSE_UP, [this]() {
				char const* filters[1] = { "*.ui" };
				const char* dat = tinyfd_openFileDialog("Select UI file", NULL, 1, filters, "XML UI (.ui)", false);
				LoadUI(dat ? dat : "");
			})
		)->AddChild(
			Create<Button>()->AddChild(
				Create<Text>()->SetText("Clear canvas", 20)->SetStyle(Vector(0.f))
			)->SetOrigin(0.5f, 0.f)->SetTransform(0.f, 120.f, -50.f, 25.f, Vector(1.f), Vector(0.5f))
			->SetEventCallback(Constants::UI::UI_ON_MOUSE_UP, [this]() {
				EditCanvas->RemoveChildren();
				EditCanvas->AddChild(
					Create<Image>()->SetStyle(Vector(0.5f))->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f, 1.f, 0.f))->SetHit(HitReg::HitTestInvisible)
				)->SetOrigin(0.5f, 0.5f)->SetTransform(0.f, 700.f, 0.f, 500.f, Vector(0.5f), Vector(0.5f));
				EditCanvas->UpdateMatrices(Vector2D());
				editing = nullptr;
			})
		)
		->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f, 1.f, 0.f))
	);
}

void Editor::RightClick(float, bool)
{
	Vector2D delta = EditCanvas->space->GetCursorDelta();

	EditCanvas->topOffset += delta.Y;
	EditCanvas->leftOffset += delta.X;

	EditCanvas->UpdateMatrices(Vector2D());
}

void Editor::Edit(UIComponent* parent, UIComponent* target, const String& name)
{
	//Button* but = Create<Button>();
	//but->AddChild(Create<Text>()->SetText(name, 16)->SetJustification(Justify::Left)->SetStyle(Vector(0.f)))
	//	->SetEventCallback(Constants::UI::UI_ON_MOUSE_DOWN, [this, target]() { 
	//		editing = target; 
	//		})->SetTransform(0.f, 0.f, 0.f, 25.f);
	////auto findResult = std::find_if(std::begin(children), std::end(children), [par](const std::pair<int, UIComponent*>& pair)
	////	{
	////		return pair.second == par;
	////	});
	//TreeCanvas->AddChildAt(0, but);
	//TreeCanvas->UpdateMatrices(Vector2D());
}

void Editor::RegisterInputs(InputComponent* com)
{
	com->RegisterKeyContinuousInput(1, &Editor::RightClick, this);
}

void Editor::SetActive(EditorItemWrapper* edit)
{
	DataCanvas->RemoveChildren();
	if (editing) editing->active = false;
	editing = edit;
	if (!edit) return;
	editing->active = true;

	std::vector<UIComponent*> childs;
	editing->item->MakeEditMenu(childs);
	for (const auto& c : childs) {
		DataCanvas->AddChild(c);
	}
	DataCanvas->UpdateMatrices(Vector2D());
}

void Editor::SaveUI(String path)
{
	String data;
	auto objs = EditCanvas->GetChildren();
	for (int i = 1; i < objs.size(); i++) {
		data += objs[i]->GetString();
	}
	FileManager::SaveData(path, data);
}

void Editor::LoadUI(String path)
{
	AddFromFile(EditCanvas, path);

	auto objs = EditCanvas->GetChildren();
	for (int i = 1; i < objs.size(); i++) {
		EditorItemWrapper* item = Create<EditorItemWrapper>();
		item->editer = this;
		item->item = objs[i];
		item->z_index = 100;
		EditCanvas->AddChild(item);
		item->parent = item->item;
	}
	//SetActive(item);
	this->EditCanvas->UpdateMatrices(Vector2D());
}
