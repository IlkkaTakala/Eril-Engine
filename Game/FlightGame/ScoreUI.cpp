#include "ScoreUI.h"
#include <UI/Panel.h>
#include <UI/Text.h>

void ScoreUI::Construct()
{
	scoreText = Create<Text>();
	AddComponent(
		Create<Panel>()->
		AddChild(
			Create<Text>()->SetText("Score: ", 50)->SetStyle(Vector(1.0f))
		)->AddChild(
			scoreText->SetText("0", 50)->SetStyle(Vector(1.f))->SetTransform(160)
		)->SetTransform(0, 0, 0, 0, { 0, 1, 1 }, {0, 1, 1})
	);
}

void ScoreUI::SetScore(int score)
{
	scoreText->SetText(std::to_string(score));
}
