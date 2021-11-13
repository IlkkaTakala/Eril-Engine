#pragma once
#include <Core.h>
#include "UIComponent.h"
class Shader;

class Panel : public UIComponent
{
public:
	Panel();
	virtual ~Panel();
	void AddChild(UIComponent* child);

	virtual void Render(const Vector2D& size) const override;
	virtual void UpdateDepth(float& depth) override;
	virtual float GetTreeDepth() const override;
private:
	Shader* temp_shader;
	std::multimap<int, UIComponent*> children;
};

