#include "WinConsole.h"
#include <Interface/WindowManager.h>
#include <thread>
#include <queue>
//#define	WIN32_LEAN_AND_MEAN
//#include <windows.h>
//#include <stdio.h>
//#include <fcntl.h>
//#include <io.h>
//#include <iostream>
//#include <fstream>
//#ifndef _USE_OLD_IOSTREAMS
//using namespace std;
//#endif
//// maximum mumber of lines the output console should have
//static const WORD MAX_CONSOLE_LINES = 500;
//
//void Console::CreateOld()
//{
//	CONSOLE_SCREEN_BUFFER_INFO coninfo;
//
//	// allocate a console for this app
//	AllocConsole();
//
//	// set the screen buffer to be big enough to let us scroll text
//	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
//	coninfo.dwSize.Y = MAX_CONSOLE_LINES;
//	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);
//
//	FILE* fDummy;
//	freopen_s(&fDummy, "CONOUT$", "w", stdout);
//	freopen_s(&fDummy, "CONOUT$", "w", stderr);
//	freopen_s(&fDummy, "CONIN$", "r", stdin);
//
//	// Note that there is no CONERR$ file
//	HANDLE hStdout = CreateFile("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
//		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//	HANDLE hStdin = CreateFile("CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
//		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//
//	SetStdHandle(STD_OUTPUT_HANDLE, hStdout);
//	SetStdHandle(STD_ERROR_HANDLE, hStdout);
//	SetStdHandle(STD_INPUT_HANDLE, hStdin);
//
//	//Clear the error state for each of the C++ standard stream objects. 
//	std::wclog.clear();
//	std::clog.clear();
//	std::wcout.clear();
//	std::cout.clear();
//	std::wcerr.clear();
//	std::cerr.clear();
//	std::wcin.clear();
//	std::cin.clear();
//
//	ios::sync_with_stdio();
//}

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#define WM_ADDLOG (WM_USER + 0x0021)
#define IDC_CMDLINE 2

static HWND m_hwnd = nullptr;
static bool bQuit = false;
static std::list<std::wstring> logs;

template<class Interface>
inline void SafeRelease(
	Interface** ppInterfaceToRelease
)
{
	if (*ppInterfaceToRelease != NULL)
	{
		(*ppInterfaceToRelease)->Release();

		(*ppInterfaceToRelease) = NULL;
	}
}


#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif



#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

class DemoApp
{
public:
	DemoApp() :
		m_pDirect2dFactory(NULL),
		m_pRenderTarget(NULL),
		m_pRedBrush(NULL),
		m_pDWriteFactory(NULL),
		m_pTextFormat(NULL),
		m_pBlackBrush(NULL),
		m_pYellowBrush(NULL),
		m_logArea(NULL),
		m_input(NULL)
	{
	}

	~DemoApp()
	{
		SafeRelease(&m_pDirect2dFactory);
		SafeRelease(&m_pRenderTarget);
		SafeRelease(&m_pRedBrush);
		SafeRelease(&m_pDWriteFactory);
		SafeRelease(&m_pTextFormat);
		SafeRelease(&m_pBlackBrush);
		SafeRelease(&m_pYellowBrush);
	}

	// Register the window class and call methods for instantiating drawing resources
	HRESULT Initialize()
	{
		HRESULT hr;

		// Initialize device-indpendent resources, such
		// as the Direct2D factory.
		hr = CreateDeviceIndependentResources();

		if (SUCCEEDED(hr))
		{
			// Register the window class.
			WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
			wcex.style = CS_HREDRAW | CS_VREDRAW;
			wcex.lpfnWndProc = DemoApp::WndProc;
			wcex.cbClsExtra = 0;
			wcex.cbWndExtra = sizeof(LONG_PTR);
			wcex.hInstance = HINST_THISCOMPONENT;
			wcex.hbrBackground = NULL;
			wcex.lpszMenuName = NULL;
			wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
			wcex.lpszClassName = "ErilConsole";

			RegisterClassEx(&wcex);


			// Because the CreateWindow function takes its size in pixels,
			// obtain the system DPI and use it to scale the window size.
			FLOAT dpiX, dpiY;

			// The factory returns the current system DPI. This is also the value it will use
			// to create its own windows.
			m_pDirect2dFactory->GetDesktopDpi(&dpiX, &dpiY);


			// Create the window.
			m_hwnd = CreateWindowW(
				L"ErilConsole",
				L"Eril Console",
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				static_cast<UINT>(ceil(640.f * dpiX / 96.f)),
				static_cast<UINT>(ceil(480.f * dpiY / 96.f)),
				NULL,
				NULL,
				HINST_THISCOMPONENT,
				this
			);
			m_logArea = CreateWindowW(
				L"STATIC",
				NULL,
				WS_CHILD | WS_VISIBLE | WS_VSCROLL,
				0,
				0,
				0,
				0,
				m_hwnd,
				NULL,
				HINST_THISCOMPONENT,
				this
			);
			m_input = CreateWindowExW(
				WS_EX_CLIENTEDGE,
				L"EDIT",
				NULL,
				WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT,
				0,
				0,
				30,
				0,
				m_hwnd,
				(HMENU)IDC_CMDLINE,
				HINST_THISCOMPONENT,
				this
			);
			HFONT font = CreateFontA(
				14,
				6,
				0,
				0,
				400,
				FALSE,
				FALSE,
				FALSE,
				ANSI_CHARSET,
				OUT_OUTLINE_PRECIS,
				CLIP_DEFAULT_PRECIS,
				PROOF_QUALITY,
				FF_MODERN,
				"Verdana");
			SendMessage(m_input, WM_SETFONT, (WPARAM)font, TRUE);
			hr = m_hwnd && m_logArea && m_input ? S_OK : E_FAIL;
			if (SUCCEEDED(hr))
			{
				ShowWindow(m_hwnd, SW_SHOWNORMAL);
				UpdateWindow(m_hwnd);
			}
		}

		return hr;
	}

	// Process and dispatch messages
	void RunMessageLoop()
	{
		MSG msg;

		while (GetMessage(&msg, NULL, 0, 0) && !bQuit)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

private:
	// Initialize device-independent resources.
	HRESULT CreateDeviceIndependentResources()
	{
		static const WCHAR msc_fontName[] = L"Verdana";
		static const FLOAT msc_fontSize = 12;
		HRESULT hr;

		// Create a Direct2D factory.
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);

		if (SUCCEEDED(hr))
		{
			// Create a DirectWrite factory.
			hr = DWriteCreateFactory(
				DWRITE_FACTORY_TYPE_SHARED,
				__uuidof(m_pDWriteFactory),
				reinterpret_cast<IUnknown**>(&m_pDWriteFactory)
			);
		}
		if (SUCCEEDED(hr))
		{
			// Create a DirectWrite text format object.
			hr = m_pDWriteFactory->CreateTextFormat(
				msc_fontName,
				NULL,
				DWRITE_FONT_WEIGHT_NORMAL,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				msc_fontSize,
				L"", //locale
				&m_pTextFormat
			);
		}
		if (SUCCEEDED(hr))
		{
			// Center the text horizontally and vertically.
			m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

			m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		}

		return hr;
	}

	// Initialize device-dependent resources.
	HRESULT CreateDeviceResources()
	{
		HRESULT hr = S_OK;

		if (!m_pRenderTarget)
		{
			RECT rc;
			GetClientRect(m_logArea, &rc);

			D2D1_SIZE_U size = D2D1::SizeU(
				rc.right - rc.left,
				rc.bottom - rc.top
			);

			// Create a Direct2D render target.
			hr = m_pDirect2dFactory->CreateHwndRenderTarget(
				D2D1::RenderTargetProperties(),
				D2D1::HwndRenderTargetProperties(m_logArea, size),
				&m_pRenderTarget
			);


			if (SUCCEEDED(hr))
			{
				// Create a gray brush.
				hr = m_pRenderTarget->CreateSolidColorBrush(
					D2D1::ColorF(D2D1::ColorF::Red),
					&m_pRedBrush
				);
			}
			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateSolidColorBrush(
					D2D1::ColorF(D2D1::ColorF::Black, 1.0f),
					&m_pBlackBrush
				);
			}

			// Create a solid color brush with its rgb value 0x9ACD32.
			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateSolidColorBrush(
					D2D1::ColorF(D2D1::ColorF::Orange),
					&m_pYellowBrush
				);
			}
		}

		return hr;
	}

	// Release device-dependent resource.
	void DiscardDeviceResources()
	{
		SafeRelease(&m_pRenderTarget);
		SafeRelease(&m_pRedBrush);
		SafeRelease(&m_pYellowBrush);
	}

	// Draw content.
	HRESULT OnRender()
	{
		HRESULT hr;

		hr = CreateDeviceResources();

		if (SUCCEEDED(hr))
		{
			// Retrieve the size of the render target.
			D2D1_SIZE_F renderTargetSize = m_pRenderTarget->GetSize();

			m_pRenderTarget->BeginDraw();

			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

			m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Gray));

			float bottom = renderTargetSize.height - 5.f;
			int fontSize = (int)m_pTextFormat->GetFontSize() + 2;
			int max_lineCount = int(bottom / fontSize) - 1;

			std::wstring text;
			int round = 1;
			if (logs.size() > 0) {
				for (auto i = --logs.end(); i != logs.begin(); i--) {
					m_pRenderTarget->DrawText(
						i->c_str(),
						i->size(),
						m_pTextFormat,
						D2D1::RectF(0, bottom - fontSize * round, renderTargetSize.width, bottom - fontSize * (round - 1)),
						i->starts_with(L"[LOG]") ? m_pBlackBrush : i->starts_with(L"[WARN]") ? m_pYellowBrush : m_pRedBrush
					);
					if (round > max_lineCount) break;
					round++;
				}
			}

			hr = m_pRenderTarget->EndDraw();

			if (hr == D2DERR_RECREATE_TARGET)
			{
				hr = S_OK;
				DiscardDeviceResources();
			}
		}

		ValidateRect(m_logArea, NULL);

		return hr;
	}

	// Resize the render target.
	void OnResize(UINT width, UINT height)
	{
		MoveWindow(m_logArea, 0, 0, width, height - 30, FALSE);
		if (m_pRenderTarget)
		{
			// Note: This method can fail, but it's okay to ignore the
			// error here, because the error will be returned again
			// the next time EndDraw is called.
			RECT rc;
			GetClientRect(m_logArea, &rc);

			D2D1_SIZE_U size = D2D1::SizeU(
				rc.right - rc.left,
				rc.bottom - rc.top
			);
			m_pRenderTarget->Resize(size);
		}
		MoveWindow(m_input, 0, height - 30, width, 30, TRUE);
	}

	// The windows procedure.
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		LRESULT result = 0;

		if (message == WM_CREATE)
		{
			LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
			DemoApp* pDemoApp = (DemoApp*)pcs->lpCreateParams;

			::SetWindowLongPtrW(
				hwnd,
				GWLP_USERDATA,
				reinterpret_cast<LONG_PTR>(pDemoApp)
			);

			result = 1;
		}
		else
		{
			DemoApp* pDemoApp = reinterpret_cast<DemoApp*>(static_cast<LONG_PTR>(
				::GetWindowLongPtrW(
					hwnd,
					GWLP_USERDATA
				)));

			bool wasHandled = false;

			if (pDemoApp)
			{
				switch (message)
				{
				case WM_SIZE:
				{
					UINT width = LOWORD(lParam);
					UINT height = HIWORD(lParam);
					pDemoApp->OnResize(width, height);
				}
				result = 0;
				wasHandled = true;
				break;

				case WM_DISPLAYCHANGE:
				{
					InvalidateRect(hwnd, NULL, FALSE);
				}
				result = 0;
				wasHandled = true;
				break;

				case WM_PAINT:
				{
					pDemoApp->OnRender();
				}
				result = 0;
				wasHandled = true;
				break;

				case WM_ADDLOG:
				{
					RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
				}
				result = 0;
				wasHandled = true;
				break;

				case WM_DESTROY:
				{
					PostQuitMessage(0);
				}
				result = 1;
				wasHandled = true;
				break;
				}
			}

			if (!wasHandled)
			{
				result = DefWindowProc(hwnd, message, wParam, lParam);
			}
		}

		return result;
	}

private:
	ID2D1Factory* m_pDirect2dFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;
	ID2D1SolidColorBrush* m_pRedBrush;
	ID2D1SolidColorBrush* m_pBlackBrush;
	ID2D1SolidColorBrush* m_pYellowBrush;
	IDWriteTextFormat* m_pTextFormat;
	IDWriteFactory* m_pDWriteFactory;
	HWND m_logArea;
	HWND m_input;
};

void D2D1Init() {
	if (SUCCEEDED(CoInitialize(NULL)))
	{
		{
			DemoApp app;

			if (SUCCEEDED(app.Initialize()))
			{
				app.RunMessageLoop();
			}
		}
		CoUninitialize();
	}
}

static std::thread ConsoleThread;

void Console::Init()
{
}

void Console::Create()
{
	ConsoleThread = std::thread(D2D1Init);
}

void Console::Close()
{
	bQuit = true;
	ConsoleThread.join();
}

void AddLine(const String& pre, const String& line)
{
	LPCSTR format = "hh':'mm':'ss";
	char w[10];
	GetTimeFormatA(LOCALE_NAME_USER_DEFAULT, TIME_NOTIMEMARKER | TIME_FORCE24HOURFORMAT, NULL, NULL, w, 9);
	std::wstringstream temp;
	temp << pre.c_str() << w << " | " << line.c_str() << '\n';
	logs.push_back(temp.str());
	SendMessage(m_hwnd, WM_ADDLOG, 0, 0);
}

void Console::Log(const String& line)
{
	AddLine("[LOG]", line);
}

void Console::Error(const String& line)
{
	AddLine("[ERROR]", line);
}

void Console::Warning(const String& line)
{
	AddLine("[WARN]", line);
}
