#include "stdafx.h"
#if defined(_WIN32)
#include "PlatformWin32.h"
#include "Core.h"
#include "MiniOpenGL11.h"
#include "ColorBuffer.h"
//=============================================================================
#if defined(_WIN32)
extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 1;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif
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

	std::chrono::steady_clock::time_point lastFrameTime{};
	float deltaTime{ 0.0f };
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
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (uMsg)
	{
	case WM_CLOSE:
		ExitEngineApp();
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_SIZE:
		{
			int width = LOWORD(lParam);
			int height = HIWORD(lParam);
			if (width > 0 && height > 0)
			{
				setWindowSize(static_cast<uint16_t>(width), static_cast<uint16_t>(height));
				if (hglrc)
				{
					glViewport(0, 0, windowWidth, windowHeight);
				}
			}
		}
		break;
	case WM_EXITSIZEMOVE:
		if (hglrc)
		{
			colorBuffer::Resize(frameBufferWidth, frameBufferHeight);
		}
		break;
	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
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

	WNDCLASSEX wndclassex{ .cbSize = sizeof(WNDCLASSEX) };
	wndclassex.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclassex.lpfnWndProc   = WindowProc;
	wndclassex.hInstance     = hInstance;
	wndclassex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wndclassex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
	wndclassex.lpszClassName = WindowClassName;
	if (!RegisterClassEx(&wndclassex))
	{
		Fatal("Failed to register window class!");
		return false;
	}

	RECT rect = { 0, 0, windowWidth, windowHeight };
	DWORD winStyleEx = WS_EX_OVERLAPPEDWINDOW;
	DWORD winStyle = WS_OVERLAPPEDWINDOW;
	AdjustWindowRectEx(&rect, winStyle, false, winStyleEx);

	hwnd = CreateWindowEx(winStyleEx, WindowClassName, windowTitle, winStyle,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left, rect.bottom - rect.top,
		nullptr, nullptr, hInstance, nullptr);
	if (!hwnd)
	{
		Fatal("Failed to create window!");
		return false;
	}
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	GetClientRect(hwnd, &rect);
	auto clientWidth = (rect.right - rect.left);
	auto clientHeight = (rect.bottom - rect.top);
	if (clientWidth < 0 || clientHeight < 0)
	{
		Fatal("Invalid window rectangle dimensions");
		return false;
	}

	windowWidth = static_cast<uint16_t>(clientWidth);
	windowHeight = static_cast<uint16_t>(clientHeight);

	if (!initOpenGL11Context())
	{
		Fatal("Failed to initialize OpenGL 1.1 context!");
		return false;
	}

	colorBuffer::Create(frameBufferWidth, frameBufferHeight);

	lastFrameTime = std::chrono::steady_clock::now();
	deltaTime = 0.0f;

	IsRunningApp = true;
	return true;
}
//=============================================================================
void engine::Shutdown()
{
	IsRunningApp = false;

	colorBuffer::Destroy();
		
	if (hglrc)
	{
		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(hglrc);
	}
	if (hwnd && hdc) ReleaseDC(hwnd, hdc);
	if (hwnd) DestroyWindow(hwnd);
	if (hInstance) UnregisterClass(WindowClassName, hInstance);

	hglrc = nullptr;
	hdc = nullptr;
	hwnd = nullptr;
	hInstance = nullptr;
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
	auto currentTime = std::chrono::steady_clock::now();
	deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
	lastFrameTime = currentTime;

	// нет смысла очищать экран, так как мы сразу же перерисовываем весь фреймбуфер
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT);
	colorBuffer::BeginDraw();
}
//=============================================================================
void engine::EndFrame()
{
	colorBuffer::EndDraw();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, 1.0f);
	glEnd();

	SwapBuffers(hdc);

	wchar_t title[100];
	float currentFPS = 1.0f / deltaTime;
	swprintf_s(title, L"Retro Engine - FPS: %.2f, DeltaTime: %.4f", currentFPS, deltaTime);
	SetWindowText(hwnd, title);
}
//=============================================================================
#endif // _WIN32