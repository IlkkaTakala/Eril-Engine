#pragma once
#include <Core.h>
#include <UI/UI.h>

struct UIMatrix;

enum class Visibility
{
	Hidden,
	Visible,
	Collapsed,
};

enum class HitReg
{
	HitTestInvisible,
	HitTestVisible
};

namespace Constants
{
	namespace UI 
	{
		constexpr uint	UI_ON_LEAVE = 0;
		constexpr uint	UI_ON_ENTER = 1;
		constexpr uint	UI_ON_HOVER = 2;
		constexpr uint	UI_ON_MOUSE_DOWN = 3;
		constexpr uint	UI_ON_MOUSE_UP = 4;
		constexpr uint	UI_ON_FOCUS = 5;
		constexpr uint	UI_ON_LOST_FOCUS = 6;

		typedef void (*UIEventCallback)(void);
	}
}

class UIComponent
{
public:
	UIComponent();
	virtual ~UIComponent();

	int GetZIndex() const { return z_index; }
	virtual float GetTreeDepth() const { return realDepth; }
	virtual void UpdateDepth(float& depth) { realDepth = depth; depth += 0.1f; recalculate = true; }
	void SetParent(UIComponent* p) { parent = p; }
	virtual void Render() = 0;
	virtual void UpdateMatrices(const Vector2D& size);
	UIComponent* SetTransform(float left = 0.f, float right = 0.f, float top = 0.f, float bottom = 0.f, Vector anchor_vert = Vector(), Vector anchor_hor = Vector());

	bool Trace(const Vector2D& point) const;

	virtual void OnLeave() { if (callbacks[Constants::UI::UI_ON_LEAVE] != nullptr) callbacks[Constants::UI::UI_ON_LEAVE](); }
	virtual void OnEnter() { if (callbacks[Constants::UI::UI_ON_ENTER] != nullptr) callbacks[Constants::UI::UI_ON_ENTER](); }
	virtual void OnHover() { if (callbacks[Constants::UI::UI_ON_HOVER] != nullptr) callbacks[Constants::UI::UI_ON_HOVER](); }
	virtual void OnMouseDown() { if (callbacks[Constants::UI::UI_ON_MOUSE_DOWN] != nullptr) callbacks[Constants::UI::UI_ON_MOUSE_DOWN](); }
	virtual void OnMouseUp() { if (callbacks[Constants::UI::UI_ON_MOUSE_UP] != nullptr) callbacks[Constants::UI::UI_ON_MOUSE_UP](); }
	virtual void OnFocus() { if (callbacks[Constants::UI::UI_ON_FOCUS] != nullptr) callbacks[Constants::UI::UI_ON_FOCUS](); }
	virtual void OnLostFocus() { if (callbacks[Constants::UI::UI_ON_LOST_FOCUS] != nullptr) callbacks[Constants::UI::UI_ON_LOST_FOCUS](); }

	UIComponent* SetEventCallback(uint event, Constants::UI::UIEventCallback call);

	virtual void HoverCheck(Vector2D& point);

	virtual void LoadWithParameters(const String& args) {}

protected:
	friend class UISpace;

	Vector origin;
	float leftOffset;
	float topOffset;
	float rightOffset;
	float bottomOffset;

	Vector anchor_v;
	Vector anchor_h;

	Visibility visible;
	HitReg hits;

	Transformation transform;

	UIMatrix* matrix;

	int z_index;
	float realDepth;
	Vector2D realSize;
	Vector2D topLeft;
	Vector2D desiredSize;

	Vector basecolor;
	Vector tint;
	float opacity;

	bool focusable;
	bool hasFocus;
	bool hovered;

	UIComponent* parent;
	UISpace* space;

	bool redraw;
	bool recalculate;

private:

	Constants::UI::UIEventCallback callbacks[7];
};

