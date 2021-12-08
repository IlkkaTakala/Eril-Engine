#pragma once
#include <UI/Image.h>

enum class ButtonState
{
	Neutral,
	Hovered,
	Pressed
};

class Button : public Image
{
	REGISTER_UI(Button)
public:
	Button();
	virtual ~Button();

	Button* SetHoverStyle(const UIStyle& s);
	Button* SetPressStyle(const UIStyle& s);
	Button* SetStyle(const UIStyle& s);
	void OnEnter() override;
	void OnLeave() override;
	void OnMouseDown() override;
	void OnMouseUp() override;

	void Render() override;

	virtual void UpdateDepth(float& depth) override;
	virtual float GetTreeDepth() const override;
	virtual void UpdateMatrices(const Vector2D& size) override;
	virtual void HoverCheck(Vector2D& point) override;

	Button* AddChild(UIComponent* child);

protected:
	UIStyle hovered;
	UIStyle pressed;
	UIStyle normal;

	uint normalBuffer;
	uint hoverBuffer;
	uint pressBuffer;

	ButtonState state;

	UIComponent* child;
};

