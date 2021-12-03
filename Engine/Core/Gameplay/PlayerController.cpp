#include "PlayerController.h"
#include <GameLoop.h>
#include <Gameplay/GameState.h>

Player::Player() : Actor()
{
	PlayerCamera = RI->CreateCamera();
	PlayerCamera->SetLocation(Vector(0.f, 0.f, 1.5f));
	RI->SetActiveCamera(PlayerCamera);
}

void Player::Tick(float)
{
}

void Player::LoadWithParameters(const String& args)
{
	Actor::LoadWithParameters(args);

	Loop->State->CurrentPlayer = this;
}
