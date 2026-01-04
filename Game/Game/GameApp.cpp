#include "stdafx.h"
#include "GameApp.h"
#include "Core.h"
#if defined(_WIN32)
#	include "PlatformWin32.h"
#	include "ColorBuffer.h"
#endif
//=============================================================================
namespace
{
	std::random_device rd;
	std::mt19937 gen{ rd() };
	std::uniform_int_distribution<> dis{ 0, 255 };
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
			
			colorBuffer::Clear(40, 100, 255);
			for (uint16_t x = 0; x < colorBuffer::GetWidth(); x++)
			{
				for (uint16_t y = 0; y < colorBuffer::GetHeight(); y++)
				{
					colorBuffer::SetPixel(x, y,
						static_cast<uint8_t>(dis(gen)),
						static_cast<uint8_t>(dis(gen)),
						static_cast<uint8_t>(dis(gen)),
						255);
				}
			}

			engine::EndFrame();
		}
	}
	engine::Shutdown();
}
//=============================================================================