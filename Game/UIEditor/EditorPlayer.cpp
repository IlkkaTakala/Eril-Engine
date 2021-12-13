#include "EditorPlayer.h"
#include "UIEditor/Editor.h"
#include "PauseUI.h"
#include <Interface/WindowManager.h>

EditorPlayer::EditorPlayer()
{
	paused = nullptr;

	II->RegisterKeyInput(256, &EditorPlayer::InputExit, this);
}


void EditorPlayer::LoadWithParameters(const String& args)
{
	auto ui = SpawnObject<Editor>();
	UI::AddToScreen(ui, this);

	WindowManager::SetShowCursor(0, true);
}

void EditorPlayer::InputExit(bool down)
{
	if (!down) return;
	if (paused == nullptr) {
		paused = SpawnObject<PauseUI>();
		UI::AddToScreen(paused, this);
	}
	else {
		paused->DestroyObject();
		paused = nullptr;
	}
}
