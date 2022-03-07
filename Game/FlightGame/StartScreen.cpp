#include "StartScreen.h"
#include <UI/Panel.h>
#include <UI/Image.h>
#include <UI/Text.h>
#include <UI/Button.h>
#include <Gameplay/Scene.h>
#include "FlightPlayer.h"
#include "Gameplay/GameState.h"

void StartScreen::Construct()
{
	AddComponent(
		Create<Panel>()->AddChild(
			Create<Image>()->SetStyle(UIStyle(Vector(0.2f), 0.9f))
			->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f, 1.f, 0.f))
		)->AddChild(
			Create<Text>()->SetText("Welcome to the Sky lands.", 30)->SetJustification(Justify::Centre)
			->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.1f), Vector(0.5f))
		)->AddChild(
			Create<Text>()->SetText("Your objective is to collect coins from the floating islands.", 30)->SetJustification(Justify::Centre)
			->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.15f), Vector(0.5f))
		)->AddChild(
			Create<Text>()->SetText("Controls: W to glide. MOUSE to look around and control direction.", 30)->SetJustification(Justify::Centre)
			->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.3f), Vector(0.5f))
		)->AddChild(
			Create<Text>()->SetText("Press begin to start playing.", 30)->SetJustification(Justify::Centre)
			->SetTransform(0.f, 0.f, -70.f, 0.f, Vector(0.5f), Vector(0.5f))
		)->AddChild(
			Create<Button>()->AddChild(
				Create<Text>()->SetText("Begin", 20)->SetStyle(Vector(0.f))
			)->SetEventCallback(Constants::UI::UI_ON_MOUSE_DOWN, []() { dynamic_cast<FlightPlayer*>(GetGameState()->CurrentPlayer.GetPointer())->InputQ(false);})
			->SetTransform(-90.f, 180.f, -40.f, 35.f, Vector(0.5f), Vector(0.5f))
		)->AddChild(
			Create<Button>()->AddChild(
				Create<Text>()->SetText("Main menu", 20)->SetStyle(Vector(0.f))
			)->SetEventCallback(Constants::UI::UI_ON_MOUSE_DOWN, []() { Scene::OpenLevel("Game/MainMenu/menu"); })
			->SetTransform(-90.f, 180.f, 0.f, 35.f, Vector(0.5f), Vector(0.5f))
		)->AddChild(
			Create<Button>()->AddChild(
				Create<Text>()->SetText("Exit", 20)->SetStyle(Vector(0.f))
			)->SetEventCallback(Constants::UI::UI_ON_MOUSE_DOWN, []() { Exit(); })
			->SetTransform(-90.f, 180.f, 40.f, 35.f, Vector(0.5f), Vector(0.5f))
		)
		->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f, 1.f, 0.f))
	);
}

