#include "TestUI.h"
#include <UI/Panel.h>
#include <UI/Image.h>

void TestUI::Construct()
{
	AddComponent(
		Create<Panel>()->AddChild(
			Create<Image>()->SetStyle(UIStyle(Vector(1.f, 0.f, 0.f), 1.0f, RI->LoadTextureByName("Assets/Textures/trees2/Leaves_d.png")))
		)
	);
}
