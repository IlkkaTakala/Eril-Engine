#pragma once
#include <UI/UI.h>
class Text;

class TestUI : public UI, public Tickable
{
public:

	virtual void Construct() override;

	void Tick(float delta) override;

private:
	Text* fpsText;
};

