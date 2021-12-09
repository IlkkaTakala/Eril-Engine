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
			)->SetEventCallback(Constants::UI::UI_ON_MOUSE_UP, [this, spawn]() {
				auto button = dynamic_cast<Button*>(this->editing);
				auto panel = dynamic_cast<Panel*>(this->editing);
				if (panel) panel->AddChild(spawn());
				else if (button) button->AddChild(spawn());
				this->EditCanvas->UpdateMatrices(Vector2D());
				})
		);
	}

	TreeCanvas = Create<VerticalPanel>();
	TreeCanvas->AddChild(
		Create<Image>()->SetStyle(Vector(0.6f))->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f, 1.f, 0.f))
	)->SetOrigin(1.f, 0.f)->SetTransform(0.f, 300.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(1.f));

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

void Editor::Edit(UIComponent* parent, UIComponent* target)
{
	TreeCanvas->AddChild();
}