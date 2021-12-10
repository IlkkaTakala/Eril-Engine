#include "UI/TextBox.h"
#include <UI/Text.h>

TextBox::TextBox()
{
	value = new Text();
	AddChild(value);
	value->SetJustification(Justify::Left);
	value->SetStyle(Vector(0.f));
}

TextBox::~TextBox()
{
}

void TextBox::Render()
{
	Button::Render();
}

void TextBox::OnFocus()
{
	II->SetTextMode(true);
	Button::OnFocus();
}

void TextBox::OnLostFocus()
{
	II->SetTextMode(false);
	Button::OnLostFocus();
}

void TextBox::OnMouseDown()
{
	Image::OnMouseDown();
}
