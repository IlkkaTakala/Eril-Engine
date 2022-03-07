#pragma once
#include "UI/UI.h"

class Text;

class ScoreUI : public UI
{
public:
	void Construct() override;
	void SetScore(int score);

private:
	Text* scoreText;
};

