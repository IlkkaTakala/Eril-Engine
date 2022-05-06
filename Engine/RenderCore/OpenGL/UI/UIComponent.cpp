#include "UIComponent.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "UISpace.h"

#include <UI/Panel.h>
#include <UI/Text.h>
#include <UI/TextBox.h>
#include <UI/Button.h>
#include <UI/VerticalPanel.h>

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
	bounds = Vector2D();

	focusable = false;
	hasFocus = false;
	hovered = false;

	parent = nullptr;
	space = IRender::GetUIManager();

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
	const Vector2D ScreenSize = IRender::GetUIManager()->GetSize();
	const glm::mat4 view = glm::ortho(0.f, (float)ScreenSize.X, (float)ScreenSize.Y, 0.f, 10.f, 0.1f);;
	const Vector loc = transform.Location;
	const Rotator rot = transform.Rotation;
	const Vector sca = transform.Scale;

	glm::vec3 gloc = parent == nullptr ? glm::vec3(0.f) : glm::vec3(parent->topLeft.X, parent->topLeft.Y, 0.f);
	glm::vec3 scale(1.f);
	glm::vec2 bound = size.X == 0 ? glm::vec2(bounds.X, bounds.Y) : glm::vec2(size.X, size.Y);
	this->bounds = Vector2D(bound.x, bound.y);

	if (anchor_v.Y <= anchor_v.X) {
		scale.y = bottomOffset;
		gloc.y += bound.y * anchor_v.X + topOffset - (bottomOffset * origin.Y);
	}
	else {
		gloc.y += anchor_v.X * bound.y + topOffset;
		scale.y = (anchor_v.Y * bound.y - bottomOffset - topOffset);
	}
	if (anchor_h.Y <= anchor_h.X) {
		scale.x = rightOffset;
		gloc.x += bound.x * anchor_h.X + leftOffset - (rightOffset * origin.X);
	}
	else {
		gloc.x += anchor_h.X * bound.x + leftOffset;
		scale.x = (anchor_h.Y * bound.x - rightOffset - leftOffset);
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

UIComponent* UIComponent::SetOrigin(float x, float y)
{
	origin.X = x;
	origin.Y = y;
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
			if (IRender::GetUIManager()->Hovered == this) IRender::GetUIManager()->Hovered = nullptr;
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
				IRender::GetUIManager()->Hovered = this;
				OnEnter();
			}
			else OnHover();
		}
		else {
			if (hovered) {
				hovered = false;
				if (IRender::GetUIManager()->Hovered == this) IRender::GetUIManager()->Hovered = nullptr;
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
		case 10: origin.Y = (float)atof(dats[9].c_str());
		case 9: origin.X = (float)atof(dats[8].c_str());
		case 8: anchor_v.Y = (float)atof(dats[7].c_str());
		case 7: anchor_v.X = (float)atof(dats[6].c_str());
		case 6: anchor_h.Y = (float)atof(dats[5].c_str());
		case 5: anchor_h.X = (float)atof(dats[4].c_str());
		case 4: bottomOffset = (float)atof(dats[3].c_str());
		case 3: topOffset = (float)atof(dats[2].c_str());
		case 2: rightOffset = (float)atof(dats[1].c_str());
		case 1: leftOffset = (float)atof(dats[0].c_str());
		case 0: break;
		}

	}
}

String UIComponent::GetString() const
{
	String data("Transform=\"");
	data += std::to_string(leftOffset) + ',' + std::to_string(rightOffset) + ',' + std::to_string(topOffset) + ',' + std::to_string(bottomOffset) + ',';
	data += std::to_string(anchor_h.X) + ',' + std::to_string(anchor_h.Y) + ',' + std::to_string(anchor_v.X) + ',' + std::to_string(anchor_v.Y) + ',';
	data += std::to_string(origin.X) + ',' + std::to_string(origin.Y);
	data += "\"";
	return data;
}

void UIComponent::MakeEditMenu(std::vector<UIComponent*>& comps)
{
	VerticalPanel* topLevel = new VerticalPanel();
	topLevel->SetTransform(0.f, 0.f, 0.f, 150.f, Vector(0.f), Vector(0.f, 1.f, 0.f));
	TextBox* offset_box = new TextBox();
	TextBox* origin_box = new TextBox();
	TextBox* anchor_box = new TextBox();
	String offs = std::to_string(leftOffset) + ',' + std::to_string(rightOffset) + ',' + std::to_string(topOffset) + ',' + std::to_string(bottomOffset);
	String ancs = std::to_string(anchor_h.X) + ',' + std::to_string(anchor_h.Y) + ',' + std::to_string(anchor_v.X) + ',' + std::to_string(anchor_v.Y);
	String ori = std::to_string(origin.X) + ',' + std::to_string(origin.Y);


	topLevel->AddChild(
		(new Panel())->AddChild(
			(new Text())->SetText("Offsets: ", 20)->SetTransform()
		)->AddChild(
			offset_box->SetText(offs)->SetFontSize(20)->SetTransform(0.f, 300.f, 0.f, 30.f, Vector(0.f), Vector(0.25f))
		)->SetTransform(0.f, 0.f, 0.f, 30.f)
	)->AddChild(
		(new Panel())->AddChild(
			(new Text())->SetText("Anchors: ", 20)->SetTransform()
		)->AddChild(
			anchor_box->SetText(ancs)->SetFontSize(20)->SetTransform(0.f, 300.f, 0.f, 30.f, Vector(0.f), Vector(0.25f))
		)->SetTransform(0.f, 0.f, 0.f, 30.f)
	)->AddChild(
		(new Panel())->AddChild(
			(new Text())->SetText("Origin: ", 20)->SetTransform()
		)->AddChild(
			origin_box->SetText(ori)->SetFontSize(20)->SetTransform(0.f, 300.f, 0.f, 30.f, Vector(0.f), Vector(0.25f))
		)->SetTransform(0.f, 0.f, 0.f, 30.f)
	)->AddChild(
		(new Panel())->SetTransform(0.f, 0.f, 0.f, 10.f)
	)->AddChild(
		(new Button())->AddChild((new Text())->SetText("Apply", 20))->SetTransform(0.f, 0.f, 0.f, 30.f)
		->SetEventCallback(Constants::UI::UI_ON_MOUSE_DOWN, [this, offset_box, anchor_box, origin_box]() {
			std::map<String, String> data;
			String trans = offset_box->GetText() + ',' + anchor_box->GetText() + ',' + origin_box->GetText();
			data.emplace("Transform", trans);
			LoadWithParameters(data);
		})
	);
	
	comps.push_back(topLevel);
}
