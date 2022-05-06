#include "TestUI.h"
#include <UI/Panel.h>
#include <UI/Image.h>
#include <UI/Text.h>
#include <UI/Button.h>
#include <UI/TextBox.h>
#include <Gameplay/Scene.h>

void TestUI::Construct()
{
	fpsText = Create<Text>();
	AddComponent(
		Create<Panel>()->AddChild(
			Create<Image>()->SetStyle(UIStyle(Vector(1.f, 0.f, 0.f), 1.0f, IRender::LoadTextureByName("Assets/Textures/trees2/Leaves_d.png")))
		)->AddChild(
			fpsText->SetStyle(UIStyle(Vector(0.f), 1.0f, nullptr))
		)
	);
}

void TestUI::Tick(float delta)
{
	fpsText->SetText(std::to_string(1.0f / delta));
}

