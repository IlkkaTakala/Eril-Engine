#include "StartScreen.h"
#include <UI/Panel.h>
#include <UI/Image.h>
#include <UI/Text.h>
#include <UI/Button.h>
#include <Gameplay/Scene.h>
#include "FlightPlayer.h"

void StartScreen::Construct()
{
	AddComponent(
		Create<Panel>()->AddChild(
			Create<Image>()->SetStyle(UIStyle(Vector(0.2f), 0.9f))
			->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f, 1.f, 0.f))
		)->AddChild(
			Create<Text>()->SetText("Welcome to the Sky lands.", 30)
			->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.1f), Vector(0.36f))
		)->AddChild(
			Create<Text>()->SetText("Your objective is to collect 5 candy canes before ghost catches you.", 30)
			->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.15f), Vector(0.13f))
		)->AddChild(
			Create<Text>()->SetText("Controls: WASD to move. E to pickup. SHIFT to sprint. MOUSE to look.", 30)
			->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.2f), Vector(0.11f))
		)->AddChild(
			Create<Text>()->SetText("Press Q to start playing.", 30)
			->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.39f), Vector(0.37f))
		)->AddChild(
			Create<Button>()->AddChild(
				Create<Text>()->SetText("Main menu", 20)->SetStyle(Vector(0.f))
			)->SetEventCallback(Constants::UI::UI_ON_MOUSE_DOWN, []() { Scene::OpenLevel("Game/MainMenu/menu"); })
			->SetTransform(-50.f, 100.f, -40.f, 25.f, Vector(0.5f), Vector(0.5f))
		)->AddChild(
			Create<Button>()->AddChild(
				Create<Text>()->SetText("Exit", 20)->SetStyle(Vector(0.f))
			)->SetEventCallback(Constants::UI::UI_ON_MOUSE_DOWN, []() { Exit(); })
			->SetTransform(-50.f, 100.f, -10.f, 25.f, Vector(0.5f), Vector(0.5f))
		)
		->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f, 1.f, 0.f))
	);
}

