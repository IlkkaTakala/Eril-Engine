#pragma once
#include <Gameplay/PlayerController.h>

class LoadingPlayer : public Player
{
	REGISTER(LoadingPlayer)
public:
	LoadingPlayer() : Player() {}
	virtual void LoadWithParameters(const String& args) override;
};

