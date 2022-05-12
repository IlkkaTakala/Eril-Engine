#include "LoadingPlayer.h"
#include "LoadingUI.h"

void LoadingPlayer::LoadWithParameters(const String& args)
{
	Player::LoadWithParameters(args);

	auto ui = SpawnObject<LoadingUI>(this);
	UI::AddToScreen(ui, this);

	SetShowCursor(false);
}