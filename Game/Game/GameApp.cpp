#include "stdafx.h"
#include "GameApp.h"
#include "Core.h"
#if defined(_WIN32)
#	include "PlatformWin32.h"
#endif
//=============================================================================
namespace
{
}
//=============================================================================
void GameApp()
{
	if (engine::Initialize(1600, 900, L"Game", 260))
	{
		while (engine::IsRunning())
		{
			if (!engine::ProcessEvents())
			{
				break;
			}
			engine::BeginFrame();
			//...
			engine::EndFrame();
		}
	}
	engine::Shutdown();
}
//=============================================================================