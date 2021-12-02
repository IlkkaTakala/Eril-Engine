#pragma once
#include <UI/Text.h>
#include <UI/Button.h>

class TextBox : public Button
{
public:
	TextBox();
	virtual ~TextBox();

	String GetText() const { return value->GetText(); }
	void SetText(String text) { value->SetText(text); }

	void Render() override;
	virtual void OnFocus() override;
	virtual void OnLostFocus() override;

protected:
	Text* value;

};

