#pragma once
#include <Core.h>
#include <UI/Image.h>

enum class Justify
{
	Left,
	Centre,
	Right
};

class Text : public Image
{
	REGISTER_UI(Text)
public:
	Text();
	virtual ~Text();

	virtual void Render() override;
	Text* SetText(const String& text, int size = 0);
	String GetText() const { return value; }
	Text* SetJustification(Justify j) { just = j; textChanged = true; return this; }

protected:
	String font;
	uint StringBuffer;
	String value;
	bool textChanged;
	int fontSize;
	int weight;
	Justify just;
};

