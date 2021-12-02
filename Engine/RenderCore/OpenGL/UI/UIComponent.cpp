#include "UIComponent.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "UISpace.h"

struct UIMatrix
{
	glm::mat4 model_m;
};

UIComponent::UIComponent()
{
	origin = Vector();
	leftOffset = 0.f;
	topOffset = 0.f;
	rightOffset = 100.f;
	bottomOffset = 50.f;

	anchor_v = Vector();
	anchor_h = Vector();

	visible = Visibility::Visible;
	hits = HitReg::HitTestInvisible;

	z_index = 0;
	realDepth = 0.f;
	desiredSize = Vector2D();

	basecolor = Vector(1.f);
	tint = Vector(1.f);
	opacity = 1.f;

	focusable = false;
	hasFocus = false;
	hovered = false;

	parent = nullptr;
	space = nullptr;

	redraw = true;
	recalculate = true;

	matrix = new UIMatrix();
}

UIComponent::~UIComponent()
{
	delete matrix;
}

void UIComponent::UpdateMatrices(const Vector2D& size)
{
	const Vector2D ScreenSize = RI->GetUIManager()->GetSize();
	const glm::mat4 view = glm::ortho(0.f, (float)ScreenSize.X, (float)ScreenSize.Y, 0.f, 10.f, 0.1f);;
	const Vector loc = transform.Location;
	const Vector rot = transform.Rotation;
	const Vector sca = transform.Scale;

	glm::vec3 gloc = parent == nullptr ? glm::vec3(0.f) : glm::vec3(parent->topLeft.X, parent->topLeft.Y, 0.f);
	glm::vec3 scale(1.f);
	glm::vec2 bounds = glm::vec2(size.X, size.Y);

	if (anchor_v.Y <= anchor_v.X) {
		scale.y = bottomOffset;
		gloc.y += bounds.y * anchor_v.X + topOffset - (bottomOffset * origin.Y);
	}
	else {
		gloc.y += anchor_v.X * bounds.y + topOffset;
		scale.y = (anchor_v.Y * bounds.y - bottomOffset);
	}
	if (anchor_h.Y <= anchor_h.X) {
		scale.x = rightOffset;
		gloc.x += bounds.x * anchor_h.X + leftOffset - (rightOffset * origin.X);
	}
	else {
		gloc.x += anchor_h.X * bounds.x + leftOffset;
		scale.x = (anchor_h.Y * bounds.x - rightOffset);
	}

	glm::mat4 model = glm::translate(glm::mat4(1.f), gloc) * glm::scale(glm::mat4(1.f), scale);

	model *= glm::translate(glm::mat4(1.0f), glm::vec3(loc.X, loc.Z, realDepth))
		* glm::toMat4(glm::quat(glm::vec3(glm::radians(rot.X), glm::radians(rot.Z), glm::radians(rot.Y))))
		* glm::scale(glm::mat4(1.0f), glm::vec3(sca.X, sca.Z, sca.Y));

	realSize.X = (long)(scale.x * sca.X);
	realSize.Y = (long)(scale.y * sca.Y);
	topLeft.X = (long)(gloc.x + loc.X);
	topLeft.Y = (long)(gloc.y + loc.Y);
	matrix->model_m = view * model;
}

UIComponent* UIComponent::SetTransform(float left, float right, float top, float bottom, Vector anchor_vert, Vector anchor_hor)
{
	leftOffset = left;
	rightOffset = right;
	topOffset = top;
	bottomOffset = bottom;
	anchor_v = anchor_vert;
	anchor_h = anchor_hor;

	return this;
}

bool UIComponent::Trace(const Vector2D& point) const
{
	if (point.X < 0.f) {
		return false;
	}
	if (point.X >= topLeft.X)
		if (point.Y >= topLeft.Y)
			if (point.X <= realSize.X + topLeft.X)
				if (point.Y <= realSize.Y + topLeft.Y)
					return true;
	return false;
}

void UIComponent::HoverCheck(Vector2D& point)
{
	if (point.X == -1) {
		if (hovered) {
			hovered = false;
			if (RI->GetUIManager()->Hovered == this) RI->GetUIManager()->Hovered = nullptr;
			OnLeave();
		}
		return;
	}
	if (hits == HitReg::HitTestVisible && visible == Visibility::Visible) {
		bool hit = Trace(point);
		if (hit)
		{
			point.X = -1;
			if (!hovered) {
				hovered = true;
				RI->GetUIManager()->Hovered = this;
				OnEnter();
			}
			else OnHover();
		}
		else {
			if (hovered) {
				hovered = false;
				if (RI->GetUIManager()->Hovered == this) RI->GetUIManager()->Hovered = nullptr;
				OnLeave();
			}
		}
	}
}
