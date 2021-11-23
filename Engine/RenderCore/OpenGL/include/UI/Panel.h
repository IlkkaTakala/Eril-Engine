#pragma once
#include <Core.h>
#include "../../OpenGL/UI/UIComponent.h"

class Panel : public UIComponent
{
public:
	Panel();
	virtual ~Panel();
	Panel* AddChild(UIComponent* child);

	virtual void Render() override;
	virtual void UpdateDepth(float& depth) override;
	virtual float GetTreeDepth() const override;
	virtual void UpdateMatrices(const Vector2D& size) override;
	virtual bool Trace(const Vector2D& point) const override;
private:
	std::multimap<int, UIComponent*> children;
};

