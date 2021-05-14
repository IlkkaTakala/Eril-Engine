#include "PlayerController.h"

Player::Player()
{
	PlayerCamera = RI->CreateCamera();
	RI->SetActiveCamera(PlayerCamera);
}

void Player::Tick(float)
{
}