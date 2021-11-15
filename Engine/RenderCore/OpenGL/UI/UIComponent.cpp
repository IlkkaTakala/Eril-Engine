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
	vertical = Vector(0.f, 100.f, 0.f);
	horizontal = Vector();

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

	matrix = nullptr;
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
	glm::mat4 view = glm::ortho(0.f, (float)size.X, (float)size.Y, 0.f, 100.f, 0.f);
	Vector loc = transform.Location;
	Vector rot = transform.Rotation;
	Vector sca = transform.Scale;



	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(loc.X, loc.Z, loc.Y))
		* glm::toMat4(glm::quat(glm::vec3(glm::radians(rot.X), glm::radians(rot.Z), glm::radians(rot.Y))))
		* glm::scale(glm::mat4(1.0f), glm::vec3(sca.X, sca.Z, sca.Y));



	matrix->model_m = view * model;
}
