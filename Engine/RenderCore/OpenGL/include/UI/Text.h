#pragma once
#include <Core.h>
#include <UI/Image.h>

class Text : public Image
{
public:
	Text();

	virtual void Render() override;

protected:
	String font;
	uint StringBuffer;
	String text;
	int fontSize;
};

