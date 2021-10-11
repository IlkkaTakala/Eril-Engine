#include "PlayerController.h"

Player::Player() : Actor()
{
	PlayerCamera = RI->CreateCamera();
	PlayerCamera->SetLocation(Vector(0.f, 0.f, 1.5f));
	RI->SetActiveCamera(PlayerCamera);
}

void Player::Tick(float)
{
}