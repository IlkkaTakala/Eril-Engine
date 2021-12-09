#pragma once
#include <UI/UI.h>

class Panel;
class VerticalPanel;

class Editor : public UI
{
public:
	virtual void Construct() override;

private:

	void Edit(UIComponent* parent, UIComponent* target);

	Panel* AddableComps;
	Panel* EditCanvas;
	Panel* DataCanvas;
	VerticalPanel* TreeCanvas;

	UIComponent* editing;
};

