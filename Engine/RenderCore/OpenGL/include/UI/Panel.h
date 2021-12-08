#pragma once
#include <Core.h>
#include "../../OpenGL/UI/UIComponent.h"

class Panel : public UIComponent
{
	REGISTER_UI(Panel)
public:
	Panel();
	virtual ~Panel();
	Panel* AddChild(UIComponent* child);

	virtual void Render() override;
	virtual void UpdateDepth(float& depth) override;
	virtual float GetTreeDepth() const override;
	virtual void UpdateMatrices(const Vector2D& size) override;
	virtual void HoverCheck(Vector2D& point) override;
private:
	std::multimap<int, UIComponent*> children;
};

