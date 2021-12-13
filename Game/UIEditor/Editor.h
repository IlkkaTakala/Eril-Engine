#pragma once
#include <UI/UI.h>

class Panel;
class VerticalPanel;
class EditorItemWrapper;

class Editor : public UI
{
public:
	Editor();
	virtual ~Editor();
	virtual void Construct() override;
	void SetActive(EditorItemWrapper* edit);

	void SaveUI(String path);
	void LoadUI(String path);
private:

	void RightClick(float, bool);
	void Edit(UIComponent* parent, UIComponent* target, const String& name);

	Panel* AddableComps;
	Panel* EditCanvas;
	Panel* DataCanvas;
	VerticalPanel* TreeCanvas;

	EditorItemWrapper* editing;
};

