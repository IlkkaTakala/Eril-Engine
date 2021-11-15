#pragma once
#include <Core.h>
#include "../../OpenGL/UI/UIComponent.h"

class Shader;

class Panel : public UIComponent
{
public:
	Panel();
	virtual ~Panel();
	Panel* AddChild(UIComponent* child);

	virtual void Render() override;
	virtual void UpdateDepth(float& depth) override;
	virtual float GetTreeDepth() const override;
private:
	Shader* temp_shader;
	std::multimap<int, UIComponent*> children;
};

