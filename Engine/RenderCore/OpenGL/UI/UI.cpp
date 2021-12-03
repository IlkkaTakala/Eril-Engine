#include <UI/UI.h>
#include "UISpace.h"

UI::UI()
{
	Owner = nullptr;
	TopLevel = nullptr;
}

void UI::OnDestroyed()
{
	RemoveFromScreen();
}

void UI::AddToScreen(UI* component, Player* Owner, int screen)
{
	if (component == nullptr) return;

	component->Owner = Owner;

	UISpace* manager = RI->GetUIManager();

	manager->AddComponent(component);
}

void UI::RemoveAllUI()
{
	UISpace* manager = RI->GetUIManager();
}

void UI::RemoveFromScreen(int screen)
{
	UISpace* manager = RI->GetUIManager();
	manager->RemoveComponent(this);
}

void UI::AddComponent(UIComponent* com)
{
	TopLevel = com;
}

