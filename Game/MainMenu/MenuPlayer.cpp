#include "MenuPlayer.h"
#include "MenuUI.h"
#include <Interface/WindowManager.h>

void MenuPlayer::LoadWithParameters(const String& args)
{
	auto ui = SpawnObject<MenuUI>();
	UI::AddToScreen(ui, this);

	WindowManager::SetShowCursor(0, true);
}
