#include "PlayerController.h"

Player::Player() : Actor()
{
	PlayerCamera = RI->CreateCamera();
	RI->SetActiveCamera(PlayerCamera);
}

void Player::Tick(float)
{
}