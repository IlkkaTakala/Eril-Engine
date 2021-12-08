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

	focusable = false;
	hasFocus = false;
	hovered = false;

	parent = nullptr;
	space = nullptr;

	redraw = true;
	recalculate = true;

	matrix = new UIMatrix();

	callbacks[0] = nullptr;
	callbacks[1] = nullptr;
	callbacks[2] = nullptr;
	callbacks[3] = nullptr;
	callbacks[4] = nullptr;
	callbacks[5] = nullptr;
	callbacks[6] = nullptr;
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

UIComponent* UIComponent::SetEventCallback(uint event, Constants::UI::UIEventCallback call)
{
	callbacks[event] = call;
	return this;  
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

void UIComponent::LoadWithParameters(const std::map<String, String>& args)
{
	if (args.find("Transform") != args.end()) {
		std::vector<String> dats = split(args.at("Transform"), ',');

		switch (dats.size())
		{
		default:
		case 10: origin.Y = atof(dats[9].c_str());
		case 9: origin.X = atof(dats[8].c_str());
		case 8: anchor_v.Y = atof(dats[7].c_str());
		case 7: anchor_v.X = atof(dats[6].c_str());
		case 6: anchor_h.Y = atof(dats[5].c_str());
		case 5: anchor_h.X = atof(dats[4].c_str());
		case 4: bottomOffset = atof(dats[3].c_str());
		case 3: topOffset = atof(dats[2].c_str());
		case 2: rightOffset = atof(dats[1].c_str());
		case 1: leftOffset = atof(dats[0].c_str());
		case 0: break;
		}

	}
}
