#pragma once
#include <UI/Panel.h>

class VerticalPanel : public Panel
{
	REGISTER_UI(VerticalPanel)
public:

	virtual void UpdateMatrices(const Vector2D& size) override;
	VerticalPanel* AddChildAt(int idx, UIComponent* com);
};

