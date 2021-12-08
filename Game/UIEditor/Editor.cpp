#include "Editor.h"
#include <UI/Panel.h>
#include <UI/Text.h>
#include <UI/Button.h>
#include <GamePlay/Scene.h>

void Editor::Construct()
{
	AddComponent(
		Create<Panel>()
		->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f, 1.f, 0.f))
	);
}
