#pragma once
#include <Core.h>

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

	bool Trace(const Vector2D& point) const;

	virtual void OnLeave() {}
	virtual void OnEnter() {}
	virtual void OnHover() {}
	virtual void HoverCheck(Vector2D& point);

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

	Vector basecolor;
	Vector tint;
	float opacity;

	bool focusable;
	bool hasFocus;
	bool hovered;

	UIComponent* parent;

	bool redraw;
	bool recalculate;
};

