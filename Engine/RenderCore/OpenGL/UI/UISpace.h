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
	Vector2D GetSize() const { return ScreenSize; }

private:

	uint UIBuffer;
	uint Color;
	uint Depth;
	uint VAO;
	uint VBO;
	uint TBO;

	Shader* UIShader;
	Shader* Combiner;

	Vector2D ScreenSize;

	std::list<Ref<UI>> TopLevel;
};

