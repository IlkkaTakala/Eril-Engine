#include "Gameplay/GameState.h"
#include "TestArea/TestPlayer.h"
#include "EngineInterface.h"
#include "GamePlay/Scene.h"

void EngineInterface::CreateDefaults()
{
    Scene::OpenLevel("Assets/Maps/demo");
}
