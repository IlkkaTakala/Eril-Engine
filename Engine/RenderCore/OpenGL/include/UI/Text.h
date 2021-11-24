#pragma once
#include <Core.h>
#include "../../OpenGL/UI/UIComponent.h"

class Text : public UIComponent
{
public:
	Text();

	virtual void Render() override;

protected:
	String font;
};

