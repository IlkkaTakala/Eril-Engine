#pragma once

#include <Core.h>

class UIComponent;
class Shader;

class UISpace
{
public:
	UISpace();
	~UISpace();

	void SetSize(uint width, uint height);
	void Render(uint Target);
	void AddComponent(UIComponent* com);

private:

	uint UIBuffer;
	uint Color;
	uint VAO;
	uint VBO;
	uint TBO;

	Shader* UIShader;
	Shader* Combiner;

	Vector2D ScreenSize;

	std::list<UIComponent*> TopLevel;
};

