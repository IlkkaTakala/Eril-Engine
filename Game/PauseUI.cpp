#include "PauseUI.h"
#include <UI/Panel.h>
#include <UI/Image.h>
#include <UI/Text.h>
#include <UI/Button.h>
#include <Gameplay/Scene.h>

void PauseUI::Construct()
{
	AddComponent(
		Create<Panel>()->AddChild(
			Create<Image>()->SetStyle(UIStyle(Vector(0.2f), 0.9f))
			->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f, 1.f, 0.f))
		)->AddChild(
			Create<Button>()->AddChild(
				Create<Text>()->SetText("Main menu", 20)->SetStyle(Vector(0.f))
			)->SetEventCallback(Constants::UI::UI_ON_MOUSE_DOWN, []() { Scene::OpenLevel("Game/MainMenu/menu"); })
			->SetTransform(-50.f, 100.f, -40.f, 25.f, Vector(0.5f), Vector(0.5f))
		)
		->AddChild(
			Create<Button>()->AddChild(
				Create<Text>()->SetText("Exit", 20)->SetStyle(Vector(0.f))
			)->SetEventCallback(Constants::UI::UI_ON_MOUSE_DOWN, []() { Exit(); })
			->SetTransform(-50.f, 100.f, -10.f, 25.f, Vector(0.5f), Vector(0.5f))
		)
		->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f, 1.f, 0.f))
	);
}
