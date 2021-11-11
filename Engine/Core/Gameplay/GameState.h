#pragma once
#include "Core.h"

class Player;
class UIHolder;

class GameState : public BaseObject
{
public:
	GameState();
	virtual ~GameState() {};
	virtual void BeginPlay() override;
	
	template <class T>
	void SetPlayer(T* player) {
		Player* p = dynamic_cast<Player*>(player);
		if (p != nullptr) CurrentPlayer = player;
	}

	Ref<Player> CurrentPlayer;
};