#include "TestUI.h"
#include <UI/Panel.h>
#include <UI/Image.h>
#include <UI/Text.h>
#include <UI/Button.h>
#include <UI/TextBox.h>

void TestUI::Construct()
{
	fpsText = Create<Text>();
	AddComponent(
		Create<Panel>()->AddChild(
			Create<Image>()->SetStyle(UIStyle(Vector(1.f, 0.f, 0.f), 1.0f, RI->LoadTextureByName("Assets/Textures/trees2/Leaves_d.png")))
		)->AddChild(
			fpsText->SetStyle(UIStyle(Vector(1.f, 0.f, 0.f), 1.0f, nullptr))
		)->AddChild(
			Create<Button>()->SetStyle(UIStyle(Vector(1.f)))->SetHoverStyle(UIStyle(Vector(0.8f)))->SetPressStyle(UIStyle(Vector(0.5f)))
			->AddChild(
				Create<Text>()->SetText("Button", 30)->SetStyle(UIStyle(Vector(0.f, 0.f, 0.f), 1.0f, nullptr))
			)->SetEventCallback(Constants::UI::UI_ON_MOUSE_DOWN,
				[]() {
					Console::Log("Clicked a button!");
				})
			->SetTransform(0, 110, 300, 30)
		)->AddChild(
			Create<TextBox>()->SetTransform(0, 110, 400, 50)
		)
	);
}

void TestUI::Tick(float delta)
{
	fpsText->SetText(std::to_string(1.0f / delta));
}

