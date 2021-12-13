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
			Create<Panel>()
				->AddChild(
					Create<Image>()->SetStyle(Vector(0.3f))
					->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f, 1.0f, 0.f))
				)->AddChild(
					Create<Image>()->SetStyle(UIStyle(Vector(1.f), 1.f, RI->LoadTextureByName("Assets/Textures/LetterLogo.png")))
					->SetTransform(20.f, 250.f, 20.f, 250.f, Vector(0.f, 0.f, 0.f), Vector(0.f, 0.f, 0.f))
				)->AddChild(
					Create<Button>()->AddChild(
						Create<Text>()->SetText("Open testing level", 25)->SetStyle(Vector(0.f))
					)->SetTransform(20.f, 40.f, 300.f, 30.f, Vector(0.f, 0.f, 0.f), Vector(0.f, 1.0f, 0.f))
					->SetEventCallback(Constants::UI::UI_ON_MOUSE_UP, []() { Scene::OpenLevel("Assets/Maps/test"); })
				)->AddChild(
					Create<Button>()->AddChild(
						Create<Text>()->SetText("Open Disco", 25)->SetStyle(Vector(0.f))
					)->SetTransform(20.f, 40.f, 350.f, 30.f, Vector(0.f, 0.f, 0.f), Vector(0.f, 1.0f, 0.f))
					->SetEventCallback(Constants::UI::UI_ON_MOUSE_UP, []() { Scene::OpenLevel("Game/Disco/disco"); })
				)->AddChild(
					Create<Button>()->AddChild(
						Create<Text>()->SetText("Open Forest Map", 25)->SetStyle(Vector(0.f))
					)->SetTransform(20.f, 40.f, 400.f, 30.f, Vector(0.f, 0.f, 0.f), Vector(0.f, 1.0f, 0.f))
					->SetEventCallback(Constants::UI::UI_ON_MOUSE_UP, []() { Scene::OpenLevel("Game/ForestGame/forest"); })
				)->AddChild(
					Create<Button>()->AddChild(
						Create<Text>()->SetText("Open UI Editor", 25)->SetStyle(Vector(0.f))
					)->SetTransform(20.f, 40.f, 450.f, 30.f, Vector(0.f, 0.f, 0.f), Vector(0.f, 1.0f, 0.f))
					->SetEventCallback(Constants::UI::UI_ON_MOUSE_UP, []() { Scene::OpenLevel("Game/UIEditor/Editor"); })
				)->AddChild(
					Create<Button>()->AddChild(
						Create<Text>()->SetText("Open UI Test", 25)->SetStyle(Vector(0.f))
					)->SetTransform(20.f, 40.f, 500.f, 30.f, Vector(0.f, 0.f, 0.f), Vector(0.f, 1.0f, 0.f))
					->SetEventCallback(Constants::UI::UI_ON_MOUSE_UP, []() { Scene::OpenLevel("Game/UITest/loaduitest"); })
				)->AddChild(
					Create<Button>()->AddChild(
						Create<Text>()->SetText("Exit", 25)->SetStyle(Vector(0.f))
					)->SetTransform(20.f, 40.f, 550.f, 30.f, Vector(0.f, 0.f, 0.f), Vector(0.f, 1.0f, 0.f))
					->SetEventCallback(Constants::UI::UI_ON_MOUSE_UP, []() { Exit(); })
				)
			->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f, 0.25f, 0.f))
		)->AddChild(
			Create<Image>()->SetStyle(UIStyle(Vector(1.f), 1.f, RI->LoadTextureByName("Assets/Textures/TextLogo.png")))
			->SetTransform(0.f, 750.f, 0.f, 512.f, Vector(0.f), Vector(0.3f))
		)->AddChild(
			Create<Text>()->SetText("v0.2.1 demo", 16)->SetJustification(Justify::Right)->SetStyle(Vector(0.5f))->SetTransform(0.f, 0.f, 5.f, 100.f, Vector(0.f, 0.f, 0.f), Vector(1.f, 1.f, 0.f))
		)
		->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f, 1.f, 0.f))
	);
}
