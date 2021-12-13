#pragma once
#include <Core.h>
#include <UI/Image.h>

class TextBox;

enum class Justify
{
	Unset,
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
	Justify GetJustification() const { return just; }
	Text* SetFont(const String& name);
	virtual void LoadWithParameters(const std::map<String, String>& args) override;
	virtual String GetString() const override;
	virtual void MakeEditMenu(std::vector<UIComponent*>& comps) override;

	virtual void UpdateMatrices(const Vector2D& size) override;

protected:
	friend class TextBox;
	String font;
	uint StringBuffer;
	String value;
	bool textChanged;
	int fontSize;
	int weight;
	Justify just;
};

