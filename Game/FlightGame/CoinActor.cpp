#include "CoinActor.h"
#include <Objects/VisibleObject.h>
#include <Objects/ColliderComponent.h>
#include <Core/Gameplay/GameState.h>
#include "FlightPlayer.h"

CoinActor::CoinActor()
{
	mesh = SpawnObject<VisibleObject>();
	mesh->SetModel("coin");
	mesh->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Assets/Materials/coin"));
	AddComponent(mesh);

	collider = SpawnObject<ColliderComponent>();
	collider->SetType(0);
	collider->SetSize(mesh->GetModel()->GetAABB());
	AddComponent(collider);
}

void CoinActor::Tick(float delta)
{
	mesh->AddRotation({ 0.f, 0.f, delta * 15 });

	Player* pla = GetGameState()->CurrentPlayer;
	if ((pla->GetLocation() - Location).Length() < 3.f) {
		dynamic_cast<FlightPlayer*>(pla)->AddScore();
		DestroyObject();
	}
}
