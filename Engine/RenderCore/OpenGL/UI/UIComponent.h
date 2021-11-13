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

	void AddToScreen() const;

	int GetZIndex() const { return z_index; }
	virtual float GetTreeDepth() const { return realDepth; }
	virtual void UpdateDepth(float& depth) { realDepth = depth; depth += 0.1f; }
	void SetParent(UIComponent* p) { parent = p; }
	virtual void Render(const Vector2D& size) const = 0;

protected:
	friend class UISpace;
	friend class UIComponent;
	virtual void UpdateMatrices();

	Vector2D origin;
	Vector2D vertical;
	Vector2D horizontal;

	Vector2D anchor_v;
	Vector2D anchor_h;

	Visibility visible;
	HitReg hits;

	Transformation transform;

	UIMatrix* matrix;

	int z_index;
	float realDepth;

	bool focusable;
	bool hasFocus;

	UIComponent* parent;

	bool redraw;
	bool recalculate;
};

