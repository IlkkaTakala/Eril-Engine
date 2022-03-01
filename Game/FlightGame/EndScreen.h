#pragma once
#include <UI/UI.h>
#include <UI/Text.h>

class EndScreen : public UI
{
	REGISTER(EndScreen)
public:
	virtual void Construct() override;
	Text* endText;
	void SetText(const String& s);
};
