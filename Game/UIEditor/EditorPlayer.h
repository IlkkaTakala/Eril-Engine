#pragma once
#include <Gameplay/PlayerController.h>

class UI;

class EditorPlayer : public Player
{
	REGISTER(EditorPlayer)
public:
	EditorPlayer();
	virtual void LoadWithParameters(const String& args) override;

private:
	void InputExit(bool);
	virtual void RegisterInputs(InputComponent* com) override;

	UI* paused;
};

