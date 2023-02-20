#pragma once
#include <UI/Text.h>
#include <UI/Button.h>

class TextBox : public Button
{
	REGISTER_UI(TextBox)
public:
	TextBox();
	virtual ~TextBox();

	String GetText() const { return value->GetText(); }
	TextBox* SetText(String text) { value->SetText(text); return this; }
	TextBox* SetFont(const String& f) { value->SetFont(f); return this; }
	TextBox* SetFontSize(int f) { value->fontSize = f; return this; }

	void Render() override;
	virtual void OnFocus() override;
	virtual void OnLostFocus() override;
	virtual void OnLeave() override;

	virtual void LoadWithParameters(const std::map<String, String>& args) override;
	virtual String GetString() const override;
protected:
	Text* value;

};

