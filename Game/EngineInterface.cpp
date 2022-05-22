#include "Gameplay/GameState.h"
#include "TestArea/TestPlayer.h"
#include "EngineInterface.h"
#include "GamePlay/Scene.h"

void EngineInterface::CreateDefaults()
{
	Scene::SetLoadingMap("Game/LoadingScreen/loading");
	Scene::OpenLevel("Assets/Maps/demo");
}
