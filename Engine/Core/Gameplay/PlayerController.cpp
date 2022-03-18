#include "PlayerController.h"
#include <GameLoop.h>
#include <Gameplay/GameState.h>

Player::Player() : Actor()
{
	PlayerCamera = IRender::CreateCamera();
	PlayerCamera->SetLocation(Vector(0.f, 0.f, 1.5f));
	IRender::SetActiveCamera(PlayerCamera);
}

void Player::Tick(float)
{
}

void Player::LoadWithParameters(const String& args)
{
	Actor::LoadWithParameters(args);

	Loop->State->CurrentPlayer = this;
	SetShowCursor(false);
}

void Player::OnDestroyed()
{
	Actor::OnDestroyed();
	delete PlayerCamera;
}

void Player::SetShowCursor(bool show, uint window)
{
	IRender::SetShowCursor(show, window);
}

bool Player::GetShowCursor(uint window)
{
	return IRender::GetShowCursor(window);;
}
