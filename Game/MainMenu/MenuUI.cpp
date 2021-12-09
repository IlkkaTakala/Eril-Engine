#include "MenuUI.h"
#include <UI/Panel.h>
#include <UI/Text.h>
#include <UI/Button.h>
#include <GamePlay/Scene.h>

void MenuUI::Construct()
{
	AddComponent(
		Create<Panel>()->AddChild(
			Create<Image>()->SetStyle(Vector(0.1f))
			->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f, 1.f, 0.f))
		)->AddChild(
			Create<Panel>()->AddChild(
				Create<Image>()->SetStyle(Vector(0.3f))
				->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f, 1.0f, 0.f))
			)->AddChild(
				Create<Button>()->AddChild(
					Create<Text>()->SetText("Open testing level", 25)->SetStyle(Vector(0.f))
				)->SetTransform(20.f, 40.f, 250.f, 30.f, Vector(0.f, 0.f, 0.f), Vector(0.f, 1.0f, 0.f))
				->SetEventCallback(Constants::UI::UI_ON_MOUSE_UP, []() { Scene::OpenLevel("Assets/Maps/test"); })
			)->AddChild(
				Create<Button>()->AddChild(
					Create<Text>()->SetText("Open UI Editor", 25)->SetStyle(Vector(0.f))
				)->SetTransform(20.f, 40.f, 350.f, 30.f, Vector(0.f, 0.f, 0.f), Vector(0.f, 1.0f, 0.f))
				->SetEventCallback(Constants::UI::UI_ON_MOUSE_UP, []() { Scene::OpenLevel("Assets/Maps/test"); })
			)->AddChild(
				Create<Button>()->AddChild(
					Create<Text>()->SetText("Open Disco", 25)->SetStyle(Vector(0.f))
				)->SetTransform(20.f, 40.f, 400.f, 30.f, Vector(0.f, 0.f, 0.f), Vector(0.f, 1.0f, 0.f))
				->SetEventCallback(Constants::UI::UI_ON_MOUSE_UP, []() { Scene::OpenLevel("Game/Disco/disco"); })
			)
			->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f, 0.25f, 0.f))
		)->AddChild(
			Create<Text>()->SetText("Eril", 150)->SetTransform(0.f, 0.f, 0.f, -300.f, Vector(0.5f, 0.4f, 0.f), Vector(0.4f, 1.f, 0.f))
		)->AddChild(
			Create<Text>()->SetText("Project", 170)->SetTransform(0.f, 0.f, 0.f, -300.f, Vector(0.25f, 0.0f, 0.f), Vector(0.4f, 1.f, 0.f))
		)->AddChild(
			Create<Text>()->SetText("v0.2.1 demo", 16)->SetJustification(Justify::Right)->SetStyle(Vector(0.5f))->SetTransform(0.f, 0.f, 0.f, 100.f, Vector(0.f, 0.f, 0.f), Vector(1.f, 1.f, 0.f))
		)
		->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f, 1.f, 0.f))
	);
}
