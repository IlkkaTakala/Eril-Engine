#include "Gameplay/GameState.h"
#include "TestPlayer.h"
#include "EngineInterface.h"
#include "GamePlay/Scene.h"

GameState* EngineInterface::CreateDefaults()
{
    Scene::OpenLevel("Assets/Maps/test");
    Ref<GameState> State = SpawnObject<GameState>();
    TestPlayer* pla = SpawnObject<TestPlayer>();
    State->SetPlayer<TestPlayer>(pla);
    return State;
}
