#pragma once
#include <Core.h>
#include <UI/Image.h>

class Text : public Image
{
public:
	Text();
	virtual ~Text();

	virtual void Render() override;
	void SetText(const String& text);
protected:
	String font;
	uint StringBuffer;
	String value;
	int fontSize;
};

