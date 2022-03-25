#include "IRender.h"
#include <Basic/QueueSafe.h>
#include <RenderCore/RenderCommands.h>
#include <Interface/WindowManager.h>
#include <GameLoop.h>
#include <condition_variable>
#include <Interface/AssetManager.h>

#ifdef OPENGL
#include "Renderer.h"
#include "Mesh.h"
#include "InputControl.h"
#endif // OPENGL


std::thread* MH = nullptr;

extern Renderer* RI = nullptr;
extern IInput* II = nullptr;
extern IMesh* MI = nullptr;

bool init = false;
SafeQueue<RenderCommand> RenderCommands;
SafeQueue<MeshCommand> MeshCommands;

int InterfaceInit()
{
	#ifdef OPENGL

	RI = new Renderer();
	II = new GLInput();
	MI = new GLMesh();

	#endif // OPENGL

	return 0;
}

void IRender::SendCommand(RenderCommand c)
{
	RenderCommands.enqueue(c);
}

void IRender::CleanRenderer()
{
	RI->CleanRenderer();
}

Camera* IRender::CreateCamera(SceneComponent* parent)
{
	return RI->CreateCamera(parent);
}

void IRender::SetActiveCamera(Camera* cam)
{
	RI->SetActiveCamera(cam);
}

Camera* IRender::GetActiveCamera()
{
	return RI->GetActiveCamera();
}

int IRender::SetupWindow(int width, int height)
{
	return RI->SetupWindow(width, height);
}

void IRender::LoadShaders()
{
	RI->LoadShaders();
}

Material* IRender::GetMaterialByName(const String& name)
{
	return RI->GetMaterialByName(name);
}

Material* IRender::LoadMaterialByName(const String& name)
{
	return RI->LoadMaterialByName(name);;
}

Texture* IRender::LoadTextureByName(const String& name)
{
	return AssetManager::LoadTextureAsyncWithPromise(name);
}

void IRender::Update()
{
	WindowManager::PollEvents();
	RI->Update(&RenderCommands, RI);
}

void IRender::Render(float delta)
{
	RI->Render(delta);
}

void IRender::GameStart()
{
	RenderCommands.enqueue({ RC_GAMESTART, 0, 0});
}

void IRender::DestroyWindow()
{
	RI->DestroyWindow();
}

UISpace* IRender::GetUIManager(int screen)
{
	return RI->GetUIManager();
}

void IRender::SetShowCursor(bool show, uint window)
{
	WindowManager::SetShowCursor(window, show);
}

bool IRender::GetShowCursor(uint window)
{
	return WindowManager::GetShowCursor(window);
}