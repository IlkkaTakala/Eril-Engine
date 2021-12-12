#include "EditorItemWrapper.h"
#include <RenderCore/OpenGL/UI/UISpace.h>
#include <UIEditor/Editor.h>

EditorItemWrapper::EditorItemWrapper()
{
	vertLock = false;
	horiLock = false;
	editer = nullptr;
	BackGround = new Image();
	BackGround->SetHit(HitReg::HitTestVisible)->SetFocusable(true);
	BackGround->SetStyle(UIStyle(Vector(0.f, 0.5f, 0.f), 1.f, RI->LoadTextureByName("Game/UIEditor/editBorder.png")))->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f, 1.f, 0.f))
		->SetEventCallback(Constants::UI::UI_ON_FOCUS, [this]() { editer->SetActive(this); })
		->SetEventCallback(Constants::UI::UI_ON_LOST_FOCUS, [this]() { active_button = -1; })
		->SetEventCallback(Constants::UI::UI_ON_MOUSE_DOWN, [this]() { active_button = 8; })
		->SetEventCallback(Constants::UI::UI_ON_MOUSE_UP, [this]() { active_button = -1; });
	BackGround->SetParent(this);

	leftOffset = 0.f;
	rightOffset = 0.f;
	topOffset = 0.f;
	bottomOffset = 0.f;
	anchor_v = Vector(0.f, 1.f, 0.f);
	anchor_h = Vector(0.f, 1.f, 0.f);
	origin = Vector(0.f, 0.f, 0.f);

	active = false;
	item = nullptr;

	active_button = -1;

	Buttons[0] = new Button();
	Buttons[0]->SetOrigin(0.5f, 0.5f)->SetTransform(0.f, 10.f, 0.f, 10.f, Vector(0.f), Vector(0.f))->SetParent(this);
	Buttons[0]->SetStyle(Vector(0.f, 1.f, 0.f))->SetHoverStyle(Vector(0.f, 0.8f, 0.f))->SetPressStyle(Vector(0.f, 0.5f, 0.f));
	Buttons[0]
		->SetEventCallback(Constants::UI::UI_ON_MOUSE_DOWN, [this]() { 
			active_button = 0;
		})->SetEventCallback(Constants::UI::UI_ON_MOUSE_UP, [this]() {
			active_button = -1;	
		})->SetEventCallback(Constants::UI::UI_ON_LOST_FOCUS, [this]() {
			active_button = -1;
		});

	Buttons[1] = new Button();
	Buttons[1]->SetOrigin(0.5f, 0.5f)->SetTransform(0.f, 10.f, 0.f, 10.f, Vector(0.f), Vector(0.5f))->SetParent(this);
	Buttons[1]->SetStyle(Vector(0.f, 1.f, 0.f))->SetHoverStyle(Vector(0.f, 0.8f, 0.f))->SetPressStyle(Vector(0.f, 0.5f, 0.f));
	Buttons[1]
		->SetEventCallback(Constants::UI::UI_ON_MOUSE_DOWN, [this]() {
			active_button = 1;
		})->SetEventCallback(Constants::UI::UI_ON_MOUSE_UP, [this]() {
			active_button = -1;
		})->SetEventCallback(Constants::UI::UI_ON_LOST_FOCUS, [this]() {
			active_button = -1;
		});

	Buttons[2] = new Button();
	Buttons[2]->SetOrigin(0.5f, 0.5f)->SetTransform(0.f, 10.f, 0.f, 10.f, Vector(0.f), Vector(1.f))->SetParent(this);
	Buttons[2]->SetStyle(Vector(0.f, 1.f, 0.f))->SetHoverStyle(Vector(0.f, 0.8f, 0.f))->SetPressStyle(Vector(0.f, 0.5f, 0.f));
	Buttons[2]
		->SetEventCallback(Constants::UI::UI_ON_MOUSE_DOWN, [this]() {
			active_button = 2;
		})->SetEventCallback(Constants::UI::UI_ON_MOUSE_UP, [this]() {
			active_button = -1;
		})->SetEventCallback(Constants::UI::UI_ON_LOST_FOCUS, [this]() {
			active_button = -1;
		});

	Buttons[3] = new Button();
	Buttons[3]->SetOrigin(0.5f, 0.5f)->SetTransform(0.f, 10.f, 0.f, 10.f, Vector(0.5f), Vector(0.f))->SetParent(this);
	Buttons[3]->SetStyle(Vector(0.f, 1.f, 0.f))->SetHoverStyle(Vector(0.f, 0.8f, 0.f))->SetPressStyle(Vector(0.f, 0.5f, 0.f));
	Buttons[3]
		->SetEventCallback(Constants::UI::UI_ON_MOUSE_DOWN, [this]() {
			active_button = 3;
		})->SetEventCallback(Constants::UI::UI_ON_MOUSE_UP, [this]() {
			active_button = -1;
		})->SetEventCallback(Constants::UI::UI_ON_LOST_FOCUS, [this]() {
			active_button = -1;
		});

	Buttons[4] = new Button();
	Buttons[4]->SetOrigin(0.5f, 0.5f)->SetTransform(0.f, 10.f, 0.f, 10.f, Vector(0.5f), Vector(1.f))->SetParent(this);
	Buttons[4]->SetStyle(Vector(0.f, 1.f, 0.f))->SetHoverStyle(Vector(0.f, 0.8f, 0.f))->SetPressStyle(Vector(0.f, 0.5f, 0.f));
	Buttons[4]
		->SetEventCallback(Constants::UI::UI_ON_MOUSE_DOWN, [this]() {
			active_button = 4;
		})->SetEventCallback(Constants::UI::UI_ON_MOUSE_UP, [this]() {
			active_button = -1;
		})->SetEventCallback(Constants::UI::UI_ON_LOST_FOCUS, [this]() {
			active_button = -1;
		});

	Buttons[5] = new Button();
	Buttons[5]->SetOrigin(0.5f, 0.5f)->SetTransform(0.f, 10.f, 0.f, 10.f, Vector(1.f), Vector(0.f))->SetParent(this);
	Buttons[5]->SetStyle(Vector(0.f, 1.f, 0.f))->SetHoverStyle(Vector(0.f, 0.8f, 0.f))->SetPressStyle(Vector(0.f, 0.5f, 0.f));
	Buttons[5]
		->SetEventCallback(Constants::UI::UI_ON_MOUSE_DOWN, [this]() {
			active_button = 5;
		})->SetEventCallback(Constants::UI::UI_ON_MOUSE_UP, [this]() {
			active_button = -1;
		})->SetEventCallback(Constants::UI::UI_ON_LOST_FOCUS, [this]() {
			active_button = -1;
		});

	Buttons[6] = new Button();
	Buttons[6]->SetOrigin(0.5f, 0.5f)->SetTransform(0.f, 10.f, 0.f, 10.f, Vector(1.f), Vector(0.5f))->SetParent(this);
	Buttons[6]->SetStyle(Vector(0.f, 1.f, 0.f))->SetHoverStyle(Vector(0.f, 0.8f, 0.f))->SetPressStyle(Vector(0.f, 0.5f, 0.f));
	Buttons[6]
		->SetEventCallback(Constants::UI::UI_ON_MOUSE_DOWN, [this]() {
			active_button = 6;
		})->SetEventCallback(Constants::UI::UI_ON_MOUSE_UP, [this]() {
			active_button = -1;
		})->SetEventCallback(Constants::UI::UI_ON_LOST_FOCUS, [this]() {
			active_button = -1;
		});

	Buttons[7] = new Button();
	Buttons[7]->SetOrigin(0.5f, 0.5f)->SetTransform(0.f, 10.f, 0.f, 10.f, Vector(1.f), Vector(1.f))->SetParent(this);
	Buttons[7]->SetStyle(Vector(0.f, 1.f, 0.f))->SetHoverStyle(Vector(0.f, 0.8f, 0.f))->SetPressStyle(Vector(0.f, 0.5f, 0.f));
	Buttons[7]
		->SetEventCallback(Constants::UI::UI_ON_MOUSE_DOWN, [this]() {
			active_button = 7;
		})->SetEventCallback(Constants::UI::UI_ON_MOUSE_UP, [this]() {
			active_button = -1;
		})->SetEventCallback(Constants::UI::UI_ON_LOST_FOCUS, [this]() {
			active_button = -1;
		});
}

EditorItemWrapper::~EditorItemWrapper()
{
	delete BackGround;
	for (int i = 0; i < 8; i++) delete Buttons[i];
}

void EditorItemWrapper::Render()
{
	if (active) {
		Vector2D delta = space->GetCursorDelta();
		switch (active_button)
		{
		case 0:
		{
			item->leftOffset += delta.X;
			item->topOffset += delta.Y;
			if (!vertLock) item->bottomOffset -= delta.Y;
			if (!horiLock) item->rightOffset -= delta.X;

			UpdateMatrices(Vector2D());
		} break;
		case 1:
		{
			item->topOffset += delta.Y;
			if (!vertLock) item->bottomOffset -= delta.Y;

			UpdateMatrices(Vector2D());
		} break;
		case 2:
		{
			item->topOffset += delta.Y;
			if (!vertLock) item->bottomOffset -= delta.Y;
			if (!horiLock) item->rightOffset += delta.X;
			else item->rightOffset -= delta.X;

			UpdateMatrices(Vector2D());
		} break;
		case 3:
		{
			item->leftOffset += delta.X;
			if (!horiLock) item->rightOffset -= delta.X;

			UpdateMatrices(Vector2D());
		} break;
		case 4:
		{
			if (!horiLock) item->rightOffset += delta.X;
			else item->rightOffset -= delta.X;

			UpdateMatrices(Vector2D());
		} break;
		case 5:
		{
			item->leftOffset += delta.X;
			if (!vertLock) item->bottomOffset += delta.Y;
			else item->bottomOffset -= delta.Y;
			if (!horiLock) item->rightOffset -= delta.X;

			UpdateMatrices(Vector2D());
		} break;
		case 6:
		{
			if (!vertLock) item->bottomOffset += delta.Y;
			else item->bottomOffset -= delta.Y;

			UpdateMatrices(Vector2D());
		} break;
		case 7:
		{
			if (!vertLock) item->bottomOffset += delta.Y;
			else item->bottomOffset -= delta.Y;
			if (!horiLock) item->rightOffset += delta.X;
			else item->rightOffset -= delta.X;
			UpdateMatrices(Vector2D());
		} break;
		case 8:
		{
			if (!horiLock) item->leftOffset += delta.X;
			else {
				item->leftOffset += delta.X;
				item->rightOffset -= delta.X;
			}
			if (!vertLock) item->topOffset += delta.Y;
			else {
				item->topOffset += delta.Y;
				item->bottomOffset -= delta.Y;
			}

			UpdateMatrices(Vector2D());
		} break;
		case -1:
		default:
			break;
		}
		BackGround->Render();
		for (int i = 0; i < 8; i++) {
			Buttons[i]->Render();
		}
	}
	else
	{
		active_button = -1;
	}
}

void EditorItemWrapper::UpdateMatrices(const Vector2D& size)
{
	item->UpdateMatrices(item->bounds);
	UIComponent::UpdateMatrices(item->realSize);
	BackGround->UpdateMatrices(realSize);
	for (int i = 0; i < 8; i++) {
		Buttons[i]->UpdateMatrices(realSize);
	}
	if (item->anchor_h.Y > item->anchor_h.X || item->anchor_v.Y > item->anchor_v.X) {
		if (item->anchor_h.Y > item->anchor_h.X) {
			horiLock = true;
		}
		else
		{
			horiLock = false;
		}
		if (item->anchor_v.Y > item->anchor_v.X) {
			vertLock = true;
		}
		else {
			vertLock = false;
		}
	}
	else {
		vertLock = false;
		horiLock = false;
	}
}

void EditorItemWrapper::HoverCheck(Vector2D& point)
{
	for (int i = 0; i < 8; i++) {
		Buttons[i]->HoverCheck(point);
	}
	BackGround->HoverCheck(point);
	UIComponent::HoverCheck(point);
}

String EditorItemWrapper::GetString() const
{
	return "";
}
