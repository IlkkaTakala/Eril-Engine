#pragma once
#include <UI/UI.h>
#include <UI/Panel.h>
#include <UI/Text.h>
#include <UI/Button.h>

class LoadingUI : public UI, public Tickable
{
public:
	Image* image;

	void Tick(float delta) override
	{
		image->transform.Rotation = image->transform.Rotation.RotateAroundAxis(delta, Vector(0.f, 0.f, 1.f));
		image->UpdateMatrices({0, 0});
	}

	virtual void Construct() override
	{
		image = Create<Image>();

		AddComponent(
			Create<Panel>()->AddChild(
				Create<Image>()->SetStyle(Vector(0.1f))
				->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f, 1.f, 0.f))
			)->AddChild(
				image->SetStyle(UIStyle(Vector(1.f), 1.f, IRender::LoadTextureByName("Assets/Textures/LetterLogo.png")))
				->SetTransform(-128.f, 128.f, -128.f, 128.f, Vector(1.f), Vector(1.f))->SetOrigin(0.5f, 0.5f)
			)->AddChild(
				Create<Text>()->SetText("v0.3.0 demo", 16)->SetJustification(Justify::Right)->SetStyle(Vector(0.5f))->SetTransform(0.f, 0.f, 5.f, 100.f, Vector(0.f, 0.f, 0.f), Vector(1.f, 1.f, 0.f))
			)->AddChild(
				Create<Text>()->SetText("Loading", 30)->SetJustification(Justify::Right)->SetStyle(Vector(0.5f))->SetOrigin(0.5f, 0.5f)
				->SetTransform(-195.f, 0.f, -143.f, 0.f, Vector(1.f), Vector(1.f))
			)->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f, 1.f, 0.f))
		);
	}
};

