#pragma once
#include <Gameplay/PlayerController.h>


class MenuPlayer : public Player
{
	REGISTER(MenuPlayer)
public:
	virtual void LoadWithParameters(const String& args) override;
};

