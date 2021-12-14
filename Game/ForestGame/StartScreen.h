#pragma once
#include <UI/UI.h>
#include <UI/Text.h>

class StartScreen : public UI
{
	REGISTER(StartScreen)
public:
	virtual void Construct() override;
};
