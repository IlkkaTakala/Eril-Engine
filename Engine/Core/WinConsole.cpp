#include "WinConsole.h"
#include <Interface/WindowManager.h>
#include <thread>
#include <queue>
#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#ifndef _USE_OLD_IOSTREAMS
using namespace std;
#endif
// maximum mumber of lines the output console should have
static const WORD MAX_CONSOLE_LINES = 500;
constexpr int max_size = MAX_CONSOLE_LINES * 200;
static std::stringstream logs;
static std::thread ConsoleThread;
static bool bQuit = false;
static std::queue<String> CommandQueue;

#define WM_ADDLOG (WM_USER + 0x1111)
enum { ID_LABEL = 1, ID_BUTTON0 };
static HWND ConsoleWin, static_label, button0;
constexpr long ID_EDITCHILD = 100;

void Console::CreateOld()
{
	CONSOLE_SCREEN_BUFFER_INFO coninfo;

	// allocate a console for this app
	AllocConsole();

	// set the screen buffer to be big enough to let us scroll text
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
	coninfo.dwSize.Y = MAX_CONSOLE_LINES;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

	FILE* fDummy;
	freopen_s(&fDummy, "CONOUT$", "w", stdout);
	freopen_s(&fDummy, "CONOUT$", "w", stderr);
	freopen_s(&fDummy, "CONIN$", "r", stdin);

	// Note that there is no CONERR$ file
	HANDLE hStdout = CreateFile("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE hStdin = CreateFile("CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	SetStdHandle(STD_OUTPUT_HANDLE, hStdout);
	SetStdHandle(STD_ERROR_HANDLE, hStdout);
	SetStdHandle(STD_INPUT_HANDLE, hStdin);

	//Clear the error state for each of the C++ standard stream objects. 
	std::wclog.clear();
	std::clog.clear();
	std::wcout.clear();
	std::cout.clear();
	std::wcerr.clear();
	std::cerr.clear();
	std::wcin.clear();
	std::cin.clear();

	ios::sync_with_stdio();
}

void AddLine() {

}

void Console::Init()
{
	
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void NewConsole() 
{
	HINSTANCE hInstance = (HINSTANCE)WindowManager::GetInstance();
	if (hInstance == nullptr) return;

	const char CLASS_NAME[] = "Console Window";

	WNDCLASSEX wcex;

	wcex.cbClsExtra = 0;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.cbWndExtra = 0;
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hInstance = hInstance;
	wcex.lpfnWndProc = WindowProc;
	wcex.lpszClassName = CLASS_NAME;
	wcex.lpszMenuName = NULL;
	wcex.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClassEx(&wcex);

	// Create the window.

	ConsoleWin = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		"Eril Console",    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (ConsoleWin == NULL)
	{
		return;
	}

	ShowWindow(ConsoleWin, 1);

	// Run the message loop.

	MSG msg = { };
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) || !bQuit)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (bQuit) break;
	}

	return;
}

void Console::Create()
{
	if (ConsoleThread.joinable()) {
		return;
	}
	ConsoleThread = std::thread(NewConsole);
}

void Console::Close()
{
	bQuit = true;
	ConsoleThread.join();
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		//FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

		EndPaint(hwnd, &ps);
		break;
	}
	case WM_CREATE:
	{
		static_label = CreateWindowEx(
			0, "STATIC",   // predefined class 
			NULL,         // no window title 
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL,
			0, 0, 0, 0,   // set size in WM_SIZE message 
			hwnd,         // parent window 
			(HMENU)ID_EDITCHILD,   // edit control ID 
			(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
			NULL);        // pointer not needed 

			// Add text to the window. 	
		SetWindowTextA(static_label, logs.str().c_str());
		break;
	}

	case WM_SIZE:
	{
		// Make the edit control the size of the window's client area. 

		MoveWindow(static_label,
			0, 0,                  // starting x- and y-coordinates 
			LOWORD(lParam),        // width of client area 
			HIWORD(lParam) - 40,        // height of client area 
			TRUE);                 // repaint window 
		break;
	}
	case WM_COMMAND: //Command from Child windows and menus are under this message

		switch (wParam) //the ID is wParam
		{
			case ID_BUTTON0: //check for our button ID
			{
				SetWindowTextW(static_label, L"TEXT");

				break;
			}
		}//switch. 

		break;

	case WM_ADDLOG:

		SetWindowTextA(static_label, logs.str().c_str());
		UpdateWindow(static_label);

		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void Console::Log(const String& line)
{
	logs << "[LOG] " + line;
	SendMessageTimeout(ConsoleWin, WM_ADDLOG, 0, 0, 0, 1000, 0);
}

void Console::Error(const String& line)
{
	logs << "[ERROR] " + line;
	SendMessageTimeout(ConsoleWin, WM_ADDLOG, 0, 0, 0, 1000, 0);
}

void Console::Warning(const String& line)
{
	logs << "[WARN] " + line;
	SendMessageTimeout(ConsoleWin, WM_ADDLOG, 0, 0, 0, 1000, 0);
}

void Console::GetLogs(int lines, char* data, size_t& datasize)
{
}

void Console::Flush()
{
	//printf("%s", logs);
}

void Console::Execute(const String& command)
{
	Log(">> " + command);
}

