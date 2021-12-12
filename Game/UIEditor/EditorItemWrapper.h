#pragma once
#include <RenderCore/OpenGL/UI/UIComponent.h>
#include <UI/Image.h>
#include <UI/Button.h>

class Editor;

class EditorItemWrapper : public UIComponent
{
public:
	EditorItemWrapper();
	virtual ~EditorItemWrapper();
	virtual void Render() override;
	virtual void UpdateMatrices(const Vector2D& size) override;
	virtual void HoverCheck(Vector2D& point) override;
	virtual String GetString() const override;
private:
	friend class Editor;

	Image* BackGround;
	Button* Buttons[8];
	UIComponent* item;
	Editor* editer;

	bool active;
	int active_button;
	bool vertLock;
	bool horiLock;
};

