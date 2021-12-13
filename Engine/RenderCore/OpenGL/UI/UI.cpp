#include <UI/UI.h>
#include "UISpace.h"
#include <RenderCore/OpenGL/UI/UIComponent.h>
#include <Interface/FileManager.h>
#include <rapidxml.hpp>
#include <UI/Panel.h>
#include <UI/Button.h>

UI::UI()
{
	Owner = nullptr;
	TopLevel = nullptr;
}

void UI::OnDestroyed()
{
	RemoveFromScreen();
	delete TopLevel;
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

void ParseChildren(rapidxml::xml_node<>* node, UIComponent* base)
{
	Panel* panel_parent = dynamic_cast<Panel*>(base);
	Button* button_parent = dynamic_cast<Button*>(base);
	if (!panel_parent && !button_parent) return;

	for (auto n = node->first_node(); n; n = n->next_sibling())
	{
		String type = n->name();
		if (UI::UIList().find(type) == UI::UIList().end()) continue;

		uint32 id = 0;
		std::map<String, String> args;
		for (auto arg_ptr = n->first_attribute(); arg_ptr; arg_ptr = arg_ptr->next_attribute()) {
			args.emplace(arg_ptr->name(), arg_ptr->value());
		}

		UIComponent* par_ui = UI::UIList()[type]();
		if (par_ui != nullptr && base != nullptr) {
			if (panel_parent) panel_parent->AddChild(par_ui);
			else if (button_parent) button_parent->AddChild(par_ui);
		}

		par_ui->LoadWithParameters(args);
		ParseChildren(n, par_ui);
	}
}


UI* UI::LoadFromFile(const String& file)
{
	String data;
	FileManager::RequestData(file, data);
	if (data == "") return nullptr;

	using namespace rapidxml;

	xml_document<>* doc = new xml_document<>();
	doc->parse<0>(data.data());

	Panel* toplevel = new Panel();
	ParseChildren(doc, toplevel);

	delete doc;

	UI* temp = new UI();
	temp->AddComponent(toplevel);

	return temp;
}

void UI::AddFromFile(Panel* parent, const String& file)
{
	String data;
	FileManager::RequestData(file, data);
	if (data == "") return;

	using namespace rapidxml;

	xml_document<>* doc = new xml_document<>();
	doc->parse<0>(data.data());

	ParseChildren(doc, parent);

	delete doc;
}

void UI::AddComponent(UIComponent* com)
{
	TopLevel = com;
}

