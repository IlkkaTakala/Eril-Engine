#pragma once

#include <Core.h>

class UI;
class Shader;

class UISpace
{
public:
	UISpace();
	~UISpace();

	void SetSize(uint width, uint height);
	void Render(uint Target);
	void AddComponent(UI* com);

private:

	uint UIBuffer;
	uint Color;
	uint VAO;
	uint VBO;
	uint TBO;

	Shader* UIShader;
	Shader* Combiner;

	Vector2D ScreenSize;

	std::list<Ref<UI>> TopLevel;
};

