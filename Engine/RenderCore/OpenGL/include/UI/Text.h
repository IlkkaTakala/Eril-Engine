#pragma once
#include <Core.h>
#include <UI/Image.h>

class Text : public Image
{
public:
	Text();
	virtual ~Text();

	virtual void Render() override;
	Text* SetText(const String& text, int size = 0);
protected:
	String font;
	uint StringBuffer;
	String value;
	bool textChanged;
	int fontSize;
	int weight;
};

