#include "TestUILoad.h"
#include <UI/Panel.h>
#include <UI/Image.h>
#include <UI/Text.h>
#include <UI/Button.h>
#include <UI/TextBox.h>
#include <Gameplay/Scene.h>

void TestUILoad::Construct()
{
	AddComponent(
		Create<Panel>()->AddChild(
			Create<Text>()->SetText("Load UI:")
		)->AddChild(
			Create<TextBox>()->SetText("")->SetFontSize(20)->SetTransform(120.f, 230.f, 0.f, 30.f)
		)->AddChild(
			Create<Button>()->AddChild(
				Create<Text>()->SetText("Load")
			)->SetTransform(400.f, 100.f, 0.f, 30.f)
		)->SetOrigin(0.f, 1.f)->SetTransform(0.f, 300.f, 0.f, 40.f, Vector(1.f), Vector(0.f))
	);
}

void TestUILoad::Tick(float delta)
{
}

