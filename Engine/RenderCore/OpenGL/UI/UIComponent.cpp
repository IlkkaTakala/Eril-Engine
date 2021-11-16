#include "UIComponent.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

struct UIMatrix
{
	glm::mat4 model_m;
};

UIComponent::UIComponent()
{
	origin = Vector();
	leftOffset = 0.f;
	topOffset = 0.f;
	rightOffset = 0.f;
	bottomOffset = 0.f;

	anchor_v = Vector();
	anchor_h = Vector();

	visible = Visibility::Hidden;
	hits = HitReg::HitTestInvisible;

	z_index = 0;
	realDepth = 0.f;

	basecolor = Vector(1.f);
	tint = Vector(1.f);
	opacity = 1.f;

	focusable = false;
	hasFocus = false;

	parent = nullptr;

	redraw = true;
	recalculate = true;

	matrix = new UIMatrix();
}

UIComponent::~UIComponent()
{
	delete matrix;
}

void UIComponent::AddToScreen() const
{
}

void UIComponent::UpdateMatrices(const Vector2D& size)
{
	glm::vec2 sizer(size.X, size.Y);
	glm::mat4 view = glm::ortho(0.f, (float)size.X, (float)size.Y, 0.f, 0.f, 100.f);;
	Vector loc = transform.Location;
	Vector rot = transform.Rotation;
	Vector sca = transform.Scale;

	anchor_h;
	anchor_v;
	origin;
	topOffset = 500.f;
	rightOffset = 500.f;

	glm::vec3 scale(1.f);
	if (anchor_v.X == anchor_v.Y)
		scale.y = topOffset;
	if (anchor_h.X == anchor_h.Y)
		scale.x = rightOffset;

	glm::mat4 model = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -(realDepth + 0.1f))) * glm::scale(glm::mat4(1.f), scale);

	model *= glm::translate(glm::mat4(1.0f), glm::vec3(loc.X, loc.Z, loc.Y))
		* glm::toMat4(glm::quat(glm::vec3(glm::radians(rot.X), glm::radians(rot.Z), glm::radians(rot.Y))))
		* glm::scale(glm::mat4(1.0f), glm::vec3(sca.X, sca.Z, sca.Y));

	matrix->model_m = view * model;
}
