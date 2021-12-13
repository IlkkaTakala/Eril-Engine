#include "TestUILoad.h"
#include <UI/Panel.h>
#include <UI/Image.h>
#include <UI/Text.h>
#include <UI/Button.h>
#include <UI/TextBox.h>
#include <Gameplay/Scene.h>

void TestUILoad::Construct()
{
	TextBox* tex = Create<TextBox>();
	AddComponent(
		Create<Panel>()->AddChild(
			Create<Text>()->SetText("Load UI:")
		)->AddChild(
			tex->SetText("Game/UITest/testingui.ui")->SetFontSize(20)->SetTransform(120.f, 230.f, 0.f, 30.f)
		)->AddChild(
			Create<Button>()->AddChild(
				Create<Text>()->SetText("Load")
			)->SetTransform(400.f, 100.f, 0.f, 30.f)->SetEventCallback(Constants::UI::UI_ON_MOUSE_UP, [tex]() {
		UI* t = UI::LoadFromFile(tex->GetText());
		UI::AddToScreen(t, nullptr);
			})
		)->SetOrigin(0.f, 1.f)->SetTransform(0.f, 300.f, 0.f, 40.f, Vector(1.f), Vector(0.f))
	);
}

void TestUILoad::Tick(float delta)
{
}

