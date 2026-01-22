#include "stdafx.h"
#include "GameApp.h"
#include "Core.h"
#include "EngineMath.h"
#if defined(_WIN32)
#	include "PlatformWin32.h"
#	include "ColorBuffer.h"
#endif
//=============================================================================
namespace
{

}
//=============================================================================
void GameApp()
{
	if (engine::Initialize(1600, 900, L"Game", 120))
	{
		while (engine::IsRunning())
		{
			if (!engine::ProcessEvents())
			{
				break;
			}
			engine::BeginFrame();
			
			colorBuffer::Clear(40, 100, 255);
			
			colorBuffer::SetPixel(1, 1, 255, 20, 20);

			colorBuffer::DrawLine(2, 2, 100, 50, 255, 100, 10);

			engine::EndFrame();
		}
	}
	engine::Shutdown();
}
//=============================================================================