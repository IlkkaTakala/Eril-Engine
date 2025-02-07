#include <UI/Panel.h>
#include "glad/gl.h"
#include <glm/gtx/transform.hpp>

struct UIMatrix
{
	glm::mat4 model_m;
};

Panel::Panel() : UIComponent()
{
	anchor_h = Vector(0.f, 1.f, 0.f);
	anchor_v = Vector(0.f, 1.f, 0.f);

	topOffset = 0.f;
	bottomOffset = 0.f;
	leftOffset = 0.f;
	rightOffset = 0.f;

	hits = HitReg::HitTestInvisible;
}

Panel::~Panel()
{
	for (const auto& c : children) {
		delete c.second;
	}
}

Panel* Panel::AddChild(UIComponent* child)
{
	child->SetParent(this);
	auto it = children.emplace(child->GetZIndex(), child);
	float depth = realDepth + 0.1f;

	for (auto i = children.begin(); i != children.end(); i++) {
		i->second->UpdateDepth(depth);
	}

	return this;
}

void Panel::RemoveChildren()
{
	for (const auto& c : children) {
		delete c.second;
	}
	children.clear();
}

int Panel::GetChildIndex(UIComponent* child)
{
	auto findResult = std::find_if(std::begin(children), std::end(children), [child](const std::pair<int, UIComponent*>& pair)
		{
			return pair.second == child;
		});

	if (findResult != children.end())
	{
		return (int)std::distance(children.begin(), findResult);
	} else
		return -1;
}

void Panel::Render()
{
	if (visible != Visibility::Visible) return;
	for (auto it = children.begin(); it != children.end(); it++) {
		it->second->Render();
	}
}

void Panel::UpdateDepth(float& depth)
{
	realDepth = depth;
	depth += 0.1f;

	for (auto i = children.begin(); i != children.end(); i++) {
		i->second->UpdateDepth(depth);
	}
	recalculate = true;
}

float Panel::GetTreeDepth() const
{
	if (children.size() > 0)
		return children.rbegin()->second->GetTreeDepth();
	return realDepth;
}

void Panel::UpdateMatrices(const Vector2D& size)
{
	UIComponent::UpdateMatrices(size);

	for (const auto& c : children) {
		c.second->UpdateMatrices(realSize);
	}
}

void Panel::HoverCheck(Vector2D& point)
{
	for (auto it = children.rbegin(); it != children.rend(); it++) {
		it->second->HoverCheck(point);
	}
	UIComponent::HoverCheck(point);
}

std::vector<UIComponent*> Panel::GetChildren()
{
	std::vector<UIComponent*> ret;
	for (auto it = children.begin(); it != children.end(); ++it) {
		ret.push_back(it->second);
	}
	return ret;
}

String Panel::GetString() const
{
	String data("<Panel ");
	data += UIComponent::GetString();
	if (children.size() > 0) {
		data += ">\n";
		for (const auto& c : children) {
			data += '\t' + c.second->GetString();
		}
		data += "</Panel>\n";
	}
	else {
		data += " />\n";
	}
	return data;
}
