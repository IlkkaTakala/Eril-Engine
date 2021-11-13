#include "UIComponent.h"
#include <glm/glm.hpp>

struct UIMatrix
{
	glm::mat4 model_m;
};

UIComponent::UIComponent()
{
	origin = Vector2D();
	vertical = Vector2D();
	horizontal = Vector2D();

	anchor_v = Vector2D();
	anchor_h = Vector2D();

	visible = Visibility::Hidden;
	hits = HitReg::HitTestInvisible;

	z_index = 0;
	realDepth = 0.f;

	focusable = false;
	hasFocus = false;

	parent = nullptr;

	redraw = true;
	recalculate = true;

	matrix = nullptr;
}

UIComponent::~UIComponent()
{
	delete matrix;
}

void UIComponent::AddToScreen() const
{
}

void UIComponent::UpdateMatrices()
{
	matrix->model_m = glm::mat4(1.f);
}
