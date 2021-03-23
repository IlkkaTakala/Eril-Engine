#pragma once
#include "Core.h"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOCOMM
#include <Windows.h>
#include <thread>
#include <condition_variable>
#include <queue>
#define NANORT_USE_CPP11_FEATURE
#include "nanosg.h"

class VisibleObject;

class Renderer
{
public:
	Renderer();
	~Renderer();
	int Run();
	int Quit();
	void SetWinSize(COORD Size);
	static void MarkRenderDirty(short Flag = REN_REQUIRESBUILD);
	static void AddRenderQueue() { RenderQueue.push(1); WaitRender.notify_all(); }
	static std::list<VisibleObject*> RenderObjects;
	static std::list<VisibleObject*> InvalidObjects;

private:
	friend class GC;
	friend class GameLoop;
	void DrawScreen(bool commit);
	void DrawUI();
	unsigned char DoBounce(Vector location, Vector direction, short bounce);
	static HANDLE hOut;
	static COORD WinSize;
	std::thread RenderThread;
	static std::condition_variable WaitRender;
	std::mutex RenderMutex;
	std::mutex list_mutex;
	std::mutex buffer_mutex;
	bool bQuitting;
	static std::queue<int> RenderQueue;
	char* RenderBuffer;
	char* UIBuffer;
	char* FinalBuffer;
	int BufferSize;
	bool ShowFPS;
	nanosg::Scene<float, Mesh> scene;
	float RenderTime;
	short max_bounces;
};
