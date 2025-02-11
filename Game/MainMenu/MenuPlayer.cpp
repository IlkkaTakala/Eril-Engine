#include "MenuPlayer.h"
#include "MenuUI.h"
#include <Interface/WindowManager.h>

void MenuPlayer::LoadWithParameters(const String& args)
{
	auto ui = SpawnObject<MenuUI>(this);
	UI::AddToScreen(ui, this);

	SetShowCursor(true);
}
