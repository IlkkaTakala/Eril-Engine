#pragma once

#include <Core.h>

class UI;
class UIComponent;
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
	void SetScreen(uint window) { Screen = window; }

	uint GetColor() const { return Color; }
	uint GetDepth() const { return Depth; }

private:
	friend UIComponent;

	void LeftClick(bool);
	void GetTextInput(uint);

	bool hasFocus;

	uint UIBuffer;
	uint Color;
	uint Depth;
	uint VAO;
	uint VBO;
	uint TBO;
	uint Screen;

	Shader* UIShader;
	Shader* Combiner;

	Vector2D ScreenSize;

	std::list<Ref<UI>> TopLevel;
	UIComponent* Focused;
	UIComponent* Hovered;
};

