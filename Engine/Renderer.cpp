#include <iostream>
#include <chrono>
#include "GameLoop.h"
#include "Objects/VisibleObject.h"
#include "Gameplay/GameState.h"
#include "Gameplay/PlayerController.h"

#define NANORT_USE_CPP11_FEATURE
#include "nanosg.h"
#include "Settings.h"
#include "Renderer.h"

HANDLE Renderer::hOut = GetStdHandle(STD_OUTPUT_HANDLE);
COORD Renderer::WinSize = COORD();
std::condition_variable Renderer::WaitRender;
std::list<VisibleObject*> Renderer::RenderObjects;
std::list<VisibleObject*> Renderer::InvalidObjects;
std::queue<int> Renderer::RenderQueue;

Renderer::Renderer()
{
	bQuitting = false;
	BufferSize = 0;
	RenderBuffer = nullptr;
	UIBuffer = nullptr;
	FinalBuffer = nullptr;
	RenderTime = 0.f;
	RenderThread = std::thread(&Renderer::Run, this);
	ShowFPS = INI->GetValue("Engine", "ShowFps") == "true";
	max_bounces = 3;
}

Renderer::~Renderer()
{
	delete[] RenderBuffer;
	delete[] UIBuffer;
	delete[] FinalBuffer;
	while (!RenderQueue.empty()) RenderQueue.pop();
	RenderObjects.clear();
}

int Renderer::Run()
{
	BufferSize = WinSize.Y * WinSize.X;
	RenderBuffer = new char[BufferSize]();
	UIBuffer = new char[BufferSize]();
	FinalBuffer = new char[BufferSize]();
	std::chrono::duration<float> duration = std::chrono::milliseconds(0);
	auto begin = std::chrono::steady_clock::now();
	auto time = std::chrono::milliseconds(10);
	while (!bQuitting) {
		auto start = std::chrono::steady_clock::now();
		if (BufferSize == 0) continue;
		bool RequiresCommit = false;
		std::unique_lock<std::mutex> lock(list_mutex);
		std::vector<nanosg::Node<float, Mesh>>* nodes = scene.GetNodes();
		for (VisibleObject* obj : InvalidObjects) {
			RenderObjects.remove(obj);
			for (int i = 0; i < nodes->size(); i++) {
				if ((*nodes)[i].GetName() == obj->Model->NodeName) { nodes->erase(nodes->begin() + i); obj->Model->AddedToScene = false; RequiresCommit = true; break; }
			}
		}
		InvalidObjects.clear();
		lock.unlock(); 
		{
			std::unique_lock<std::mutex> buf_lock(buffer_mutex);
			DrawScreen(RequiresCommit);
			DrawUI();
			
			for (int y = 0; y < WinSize.Y; y++) {
				for (int x = 0; x < WinSize.X; x++) {
					FinalBuffer[y * WinSize.X + x] = UIBuffer[y * WinSize.X + x] == 0 ? RenderBuffer[y * WinSize.X + x] : UIBuffer[y * WinSize.X + x];
				}
			}
		}
		
		{
			std::unique_lock<std::mutex> buf_lock(buffer_mutex);
			std::cout.write(FinalBuffer, BufferSize - 1);
		}
		duration = std::chrono::steady_clock::now() - start;
		RenderTime = 1.f / duration.count();
	}
	return 0;
}

int Renderer::Quit()
{
	bQuitting = true;
	Renderer::MarkRenderDirty();
	RenderThread.join();
	return 0;
}

void Renderer::SetWinSize(COORD Size)
{
	WinSize = Size;
	std::unique_lock<std::mutex> buf_lock(buffer_mutex);
	delete[] RenderBuffer;
	delete[] UIBuffer;
	delete[] FinalBuffer;
	BufferSize = WinSize.Y * WinSize.X;
	RenderBuffer = new char[BufferSize]();
	UIBuffer = new char[BufferSize]();
	FinalBuffer = new char[BufferSize]();
	Renderer::MarkRenderDirty();
}

void Renderer::MarkRenderDirty(short Flag)
{
	RenderQueue.push(Flag);
	WaitRender.notify_all();
}

void Renderer::DrawScreen(bool commit)
{
	GameState* game = GameLoop::State;
	SetConsoleCursorPosition(hOut, COORD());
	if (game == nullptr || game->CurrentPlayer == nullptr) return;
	const unsigned char wall = 219;
	const unsigned char wall2 = 178;
	const unsigned char wall3 = 177;
	const unsigned char wall4 = 176;
	const unsigned char shade = 111;
	const unsigned char empty = 255;

	if (WinSize.X * WinSize.Y != BufferSize) return;

	for (int i = BufferSize - 1; i >= 0; i--) RenderBuffer[i] = empty;
	Vector up = game->CurrentPlayer->UpVector();
	Vector right = game->CurrentPlayer->RightVector();

	bool Commit = commit;

	for (VisibleObject* obj : RenderObjects) {
		bool found = false;
		for (VisibleObject* old : InvalidObjects) {
			if (old == obj) {
				found = true; break;
			}
		}
		if (found) continue;
		Mesh* mesh = obj->Model;
		if (mesh == nullptr || !mesh->ModelLoaded || obj->bMarked) continue;
		if (!mesh->AddedToScene) {
			nanosg::Node<float, Mesh> node(mesh);
			node.SetLocalXform(mesh->transform);
			node.SetName(mesh->NodeName);
			scene.AddNode(node);
			Commit = true;
			mesh->AddedToScene = true;
		}
		if (mesh->RequiresBuild) {
			nanosg::Node<float, Mesh>* node;
			scene.FindNode(mesh->NodeName, &node);
			node->SetLocalXform(mesh->transform);
			Commit = true;
			mesh->RequiresBuild = false;
		}
	}
	if (scene.GetNodes()->size() < 1) return;
	if (Commit) scene.Commit();

	nanort::Ray<float> s_ray;
	s_ray.min_t = 10.f;
	s_ray.max_t = 10000.f;
	nanort::Ray<float> ray;
	ray.min_t = 1.f;
	ray.max_t = 100000000.f;
	Vector point = game->CurrentPlayer->CameraPoint;
	ray.org[0] = game->CurrentPlayer->CameraPoint.X;
	ray.org[1] = game->CurrentPlayer->CameraPoint.Y;
	ray.org[2] = game->CurrentPlayer->CameraPoint.Z;
	Vector sun = game->Sun * -1;

	for (int y = 0; y < WinSize.Y; y++) {
		for (int x = 0; x < WinSize.X; x++) {
			Vector upDir = up * ((float)(WinSize.Y - y) / WinSize.Y - 0.5f);
			Vector rightDir = right * ((float)x / WinSize.X - 0.5f);
			Vector dir = (game->CurrentPlayer->CameraDirection + upDir + rightDir).Normalize();
			
			ray.dir[0] = dir.X;
			ray.dir[1] = dir.Y;
			ray.dir[2] = dir.Z;

			RenderBuffer[y * WinSize.X + x] = DoBounce(point, dir, 1);
		}
	}
	
}

void Renderer::DrawUI()
{
	for (int i = BufferSize - 1; i >= 0; i--) UIBuffer[i] = 0;
	if (GameLoop::State == nullptr || GameLoop::State->CurrentPlayer == nullptr || GameLoop::State->CurrentPlayer->UI == nullptr) return;
	GameLoop::State->CurrentPlayer->UI->DrawUI(WinSize.X, WinSize.Y, UIBuffer);

	if (ShowFPS) {
		char buffer[13];
		char timebuffer[11];
		snprintf(buffer, sizeof buffer, "ticks: %f", Loop->fps);
		snprintf(timebuffer, sizeof timebuffer, "fps: %f", RenderTime);
		strcpy_s(UIBuffer, sizeof buffer, buffer);
		strcpy_s(&UIBuffer[WinSize.Y], sizeof timebuffer, timebuffer);
	}
}

unsigned char Renderer::DoBounce(Vector location, Vector direction, short bounce)
{
	unsigned char hit;
	unsigned short rough;

	nanort::Ray<float> bounce_ray;
	bounce_ray.min_t = 10.f;
	bounce_ray.max_t = 100000.f;
	bounce_ray.org[0] = location.X;
	bounce_ray.org[1] = location.Y;
	bounce_ray.org[2] = location.Z;
	bounce_ray.dir[0] = direction.X;
	bounce_ray.dir[1] = direction.Y;
	bounce_ray.dir[2] = direction.Z;

	nanosg::Intersection<float> isect;

	if (scene.Traverse<nanosg::Intersection<float>, nanort::TriangleIntersector<float, nanosg::Intersection<float>>>(bounce_ray, &isect, false)) {
		auto mesh = (*scene.GetNodes())[isect.node_id].GetMesh();
		hit = mesh->mat;
		rough = mesh->roughness;
		if (bounce <= max_bounces)
		{
			Vector dir(direction - 2 * (Vector(isect.Ns[0], isect.Ns[1], isect.Ns[2])));
			//auto mult = 2 * (direction.X * isect.Ns[0] + direction.Y * isect.Ns[1] + direction.Z * isect.Ns[2]);
			//Vector dir(direction.X - mult * isect.Ns[0], direction.Y - mult * isect.Ns[1], direction.Z - mult * isect.Ns[2]);
			auto nehit = DoBounce(Vector(isect.P[0], isect.P[1], isect.P[2]), dir, ++bounce);
			return nehit == 255 ? hit : nehit;
		}
		else return hit;
	}
	
	else return 255;
}

