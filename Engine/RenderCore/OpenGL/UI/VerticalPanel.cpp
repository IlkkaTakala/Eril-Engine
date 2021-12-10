#include <UI/VerticalPanel.h>

void VerticalPanel::UpdateMatrices(const Vector2D& size)
{
	Panel::UpdateMatrices(size);

	float top = 0.f;
	for (auto& [z, c] : children) {
		c->SetTransform(0.f, 0.f, top, c->bottomOffset, Vector(0.f), Vector(0.f, 1.f, 0.f));
		top += c->bottomOffset;
		c->UpdateMatrices(realSize);
	}
}

VerticalPanel* VerticalPanel::AddChildAt(int idx, UIComponent* com)
{
	com->SetParent(this);
	auto findResult = std::next(children.begin(), idx);
	auto it = children.emplace_hint(findResult, com->GetZIndex(), com);
	float depth = realDepth + 0.1f;

	for (auto i = children.begin(); i != children.end(); i++) {
		i->second->UpdateDepth(depth);
	}

	return this;
}
