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


std::thread* RI = nullptr;
std::thread* MH = nullptr;

extern RenderHandler* RH = nullptr;
extern IInput* II = nullptr;
extern IMesh* MI = nullptr;

bool init = false;
SafeQueue<RenderCommand> RenderCommands;
SafeQueue<MeshCommand> MeshCommands;

int InterfaceInit()
{
	#ifdef OPENGL

	RH = new Renderer();
	IRender::Init();
	II = new GLInput();
	MI = new GLMesh();

	#endif // OPENGL

	return 0;
}

void IRender::Init()
{
	RI = new std::thread(&Renderer::Update, &RenderCommands, (Renderer*)RH);
}

void IRender::SendCommand(RenderCommand c)
{
	RenderCommands.enqueue(c);
}

void IRender::CleanRenderer()
{
	RenderCommands.enqueue({ RC_DESTROY, 0, 0 });
}

Camera* IRender::CreateCamera(SceneComponent* parent)
{
	return RH->CreateCamera(parent);
}

void IRender::SetActiveCamera(Camera* cam)
{
	RH->SetActiveCamera(cam);
}

Camera* IRender::GetActiveCamera()
{
	return RH->GetActiveCamera();
}

int IRender::SetupWindow(int width, int height)
{
	RenderCommands.enqueue({ RC_SETUP, (uint64)width, (uint64)height });
	std::unique_lock<std::mutex> lk(RH->LoadMutex);
	RH->Condition.wait(lk);
	return 0;
}

void IRender::LoadShaders()
{
	RenderCommands.enqueue({ RC_LOADSHADERS, 0, 0 });
}

Material* IRender::GetMaterialByName(const String& name)
{
	return RH->GetMaterialByName(name);
}

Material* IRender::LoadMaterialByName(const String& name)
{
	return RH->LoadMaterialByName(name);;
}

Texture* IRender::LoadTextureByName(const String& name)
{
	return AssetManager::LoadTextureAsyncWithPromise(name);
}

void IRender::Update()
{
	RenderCommands.enqueue({ RC_RECALCULATE, 0, 0 });
	RenderCommands.enqueue({ RC_RELIGHTS, 0, 0 });
}

void IRender::Render(float delta)
{
}

void IRender::GameStart()
{
	RenderCommands.enqueue({ RC_GAMESTART, 0, 0});
}

void IRender::DestroyWindow()
{
	RenderCommands.enqueue({ RC_DESTROY, 0, 0 });
	RI->join();
}

UISpace* IRender::GetUIManager(int screen)
{
	return RH->GetUIManager();
}

void IRender::SetShowCursor(bool show, uint window)
{
	RenderCommands.enqueue({ RC_SHOWCURSOR, show, (uint64)window });
}

bool IRender::GetShowCursor(uint window)
{
	return WindowManager::GetShowCursor(window);
}