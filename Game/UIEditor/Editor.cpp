#include "Editor.h"
#include <UI/Panel.h>
#include <UI/Text.h>
#include <UI/Button.h>
#include <UI/VerticalPanel.h>
#include <GamePlay/Scene.h>

void Editor::Construct()
{
	EditCanvas = Create<Panel>();
	EditCanvas->AddChild(
		Create<Image>()->SetStyle(Vector(0.5f))->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f, 1.f, 0.f))
	)->SetOrigin(0.5f, 0.5f)->SetTransform(0.f, 500.f, 0.f, 500.f, Vector(0.5f), Vector(0.5f));
	
	editing = EditCanvas;

	DataCanvas = Create<Panel>(); 
	DataCanvas->AddChild(
		Create<Image>()->SetStyle(Vector(0.7f))->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f, 1.f, 0.f))
	)->SetOrigin(1.f, 0.f)->SetTransform(0.f, 300.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(1.f));

	AddableComps = Create<VerticalPanel>();

	for (const auto& [name, spawn] : UI::UIList()) {
		AddableComps->AddChild(
			Create<Button>()->AddChild(
				Create<Text>()->SetText(name)->SetStyle(Vector(0.f))
			)->SetEventCallback(Constants::UI::UI_ON_MOUSE_UP, [this, name, spawn]() {
				auto button = dynamic_cast<Button*>(this->editing);
				auto panel = dynamic_cast<Panel*>(this->editing);
				UIComponent* next = spawn();
				if (panel) panel->AddChild(next);
				else if (button) button->AddChild(next);
				this->Edit(this->editing, next, name);
				this->EditCanvas->UpdateMatrices(Vector2D());
			})
		);
	}

	TreeCanvas = Create<VerticalPanel>();

	AddComponent(
		Create<Panel>()->AddChild(
			EditCanvas
		)->AddChild(
			DataCanvas
		)->AddChild(
			Create<Panel>()->AddChild(
				Create<Image>()->SetStyle(Vector(0.2f))->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f, 1.f, 0.f))
			)->AddChild(
				AddableComps->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 0.5f, 0.f), Vector(0.f, 1.f, 0.f))
			)->AddChild(
				TreeCanvas->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.5f, 1.f, 0.f), Vector(0.f, 1.f, 0.f))
			)->SetTransform(0.f, 200.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f))
		)
		->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f, 1.f, 0.f))
	);
}

void Editor::Edit(UIComponent* parent, UIComponent* target, const String& name)
{
	Button* but = Create<Button>();
	but->AddChild(Create<Text>()->SetText(name, 16)->SetJustification(Justify::Left)->SetStyle(Vector(0.f)))
		->SetEventCallback(Constants::UI::UI_ON_MOUSE_DOWN, [this, target]() { 
			editing = target; 
			});
	TreeCanvas->AddChildAt(parent, but);
	TreeCanvas->UpdateMatrices(Vector2D());
}