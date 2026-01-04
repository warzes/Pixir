#include "stdafx.h"
#if defined(_WIN32)
#include "PlatformWin32.h"
#include "Core.h"
#include "MiniOpenGL11.h"
//=============================================================================
namespace
{
	uint16_t windowWidth;
	uint16_t windowHeight;
	uint16_t frameBufferWidth;
	uint16_t frameBufferHeight;

	bool IsRunningApp{ false };

	constexpr const wchar_t* WindowClassName{ L"RetroEngineWindowClass" };
	HINSTANCE hInstance{ nullptr };
	HWND hwnd{ nullptr };
	HGLRC hglrc{ nullptr };
	HDC hdc{ nullptr };
	MSG msg{};
}
//=============================================================================
void setWindowSize(uint16_t width, uint16_t height)
{
	windowWidth = width;
	windowHeight = height;

	const float aspect = static_cast<float>(width) / static_cast<float>(height);
	frameBufferWidth = static_cast<uint16_t>(static_cast<float>(frameBufferHeight) * aspect);
}
//=============================================================================
bool initOpenGL11Context()
{
	hdc = GetDC(hwnd);
	if (!hdc)
	{
		Fatal("Failed to get device context!");
		return false;
	}

	PIXELFORMATDESCRIPTOR pfd{ .nSize = sizeof(PIXELFORMATDESCRIPTOR) };
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int pixelFormat = ChoosePixelFormat(hdc, &pfd);
	if (pixelFormat == 0)
	{
		Fatal("Failed to choose pixel format!");
		return false;
	}

	if (!SetPixelFormat(hdc, pixelFormat, &pfd))
	{
		Fatal("Failed to set pixel format!");
		return false;
	}

	hglrc = wglCreateContext(hdc);
	if (!hglrc)
	{
		Fatal("Failed to create OpenGL rendering context!");
		return false;
	}

	if (!wglMakeCurrent(hdc, hglrc))
	{
		Fatal("Failed to make OpenGL rendering context current!");
		return false;
	}

	if (!InitMiniOpenGL11Func())
	{
		Fatal("Failed to initialize MiniOpenGL11 functions!");
		return false;
	}

	glViewport(0, 0, windowWidth, windowHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	return true;
}
//=============================================================================
void ExitEngineApp()
{
	IsRunningApp = false;
}
//=============================================================================
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (uMsg)
	{
	case WM_CLOSE:
		ExitEngineApp();
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}
//=============================================================================
bool engine::Initialize(uint16_t wndWidth, uint16_t wndHeight, const wchar_t* windowTitle, uint16_t fbHeight)
{
	// Validate parameters
	{
		if (windowTitle == nullptr)
		{
			Fatal("Window title cannot be null!");
			return false;
		}
		if (wndWidth == 0)
		{
			Fatal("Window width cannot be zero!");
			return false;
		}
		if (wndHeight == 0)
		{
			Fatal("Window height cannot be zero!");
			return false;
		}
		if (fbHeight == 0)
		{
			Fatal("Frame buffer height cannot be zero!");
			return false;
		}
		if (fbHeight > wndHeight)
		{
			Warning("Frame buffer height is greater than window height. Clamping frame buffer height to window height.");
			fbHeight = wndHeight;
		}
	}

	frameBufferHeight = fbHeight;
	setWindowSize(wndWidth, wndHeight);

	hInstance = GetModuleHandle(nullptr);

	WNDCLASS wc{};
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc   = WindowProc;
	wc.hInstance     = hInstance;
	wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
	wc.lpszClassName = WindowClassName;
	if (!RegisterClass(&wc))
	{
		Fatal("Failed to register window class!");
		return false;
	}

	hwnd = CreateWindow(WindowClassName, windowTitle,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		windowWidth, windowHeight,
		nullptr, nullptr, hInstance, nullptr);
	if (!hwnd)
	{
		Fatal("Failed to create window!");
		return false;
	}

	if (!initOpenGL11Context())
	{
		Fatal("Failed to initialize OpenGL 1.1 context!");
		return false;
	}

	IsRunningApp = true;
	return true;
}
//=============================================================================
void engine::Shutdown()
{
	IsRunningApp = false;
}
//=============================================================================
bool engine::IsRunning()
{
	return IsRunningApp;
}
//=============================================================================
bool engine::ProcessEvents()
{
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return false;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return IsRunningApp;
}
//=============================================================================
void engine::BeginFrame()
{

}
//=============================================================================
void engine::EndFrame()
{

}
//=============================================================================
#endif // _WIN32