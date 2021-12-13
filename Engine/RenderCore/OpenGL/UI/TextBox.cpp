#include "UI/TextBox.h"
#include <UI/Text.h>
#include "Texture.h"

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
	state = ButtonState::Pressed;
	Button::OnFocus();
}

void TextBox::OnLostFocus()
{
	II->SetTextMode(false);
	state = ButtonState::Neutral;
	Button::OnLostFocus();
}

void TextBox::OnLeave()
{
	if (state != ButtonState::Pressed) state = ButtonState::Neutral;
	Image::OnLeave();
}

void TextBox::LoadWithParameters(const std::map<String, String>& args)
{
	Button::LoadWithParameters(args);

	value->LoadWithParameters(args);
}

String TextBox::GetString() const
{
	String data("<TextBox ");
	data += UIComponent::GetString();
	data += " NormalStyle=\"" + style.Color.ToString() + ',' + std::to_string(style.Opacity) + ',' + (style.texture ? style.texture->GetName() : " ") + ',' + style.Tint.ToString() + '"';
	data += " HoverStyle=\"" + hovered.Color.ToString() + ',' + std::to_string(hovered.Opacity) + ',' + (hovered.texture ? hovered.texture->GetName() : " ") + ',' + hovered.Tint.ToString() + '"';
	data += " PressStyle=\"" + pressed.Color.ToString() + ',' + std::to_string(pressed.Opacity) + ',' + (pressed.texture ? pressed.texture->GetName() : " ") + ',' + pressed.Tint.ToString() + '"';
	data += " Text=\"" + value->value + "\"";
	data += " TextStyle=\"" + value->fontSize + ',' + value->font + ',' + std::to_string((uint8)value->just) + "\"";
	data += " />\n";
	return data;
}
