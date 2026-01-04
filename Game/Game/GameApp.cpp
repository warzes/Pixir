#include "stdafx.h"
#include "GameApp.h"
#include "Core.h"
#include "EngineMath.h"
#if defined(_WIN32)
#	include "PlatformWin32.h"
#	include "ColorBuffer.h"
#endif
//=============================================================================
struct Triangle final
{
	Vec3 p[3];
};
//=============================================================================
struct Mesh final
{
	std::vector<Triangle> triangles;
};
//=============================================================================
namespace
{
	std::random_device rd;
	std::mt19937 gen{ rd() };
	std::uniform_int_distribution<> dis{ 0, 255 };

	Mesh cubeMesh;
	Mat4 matProj;
	float fTheta{ 0.0f };
}
//=============================================================================
void GameApp()
{
	if (engine::Initialize(1600, 900, L"Game", 240))
	{
		cubeMesh.triangles = {
			// SOUTH
			{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

			// EAST                                                      
			{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
			{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

			// NORTH                                                     
			{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
			{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

			// WEST                                                      
			{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

			// TOP                                                       
			{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
			{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

			// BOTTOM                                                    
			{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
			{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },
		};

		float fNear = 0.1f;
		float fFar = 1000.0f;
		float fFov = 90.0f;
		float fAspectRatio = static_cast<float>(colorBuffer::GetHeight()) / static_cast<float>(colorBuffer::GetWidth());
		float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

		matProj.m[0][0] = fAspectRatio * fFovRad;
		matProj.m[1][1] = fFovRad;
		matProj.m[2][2] = fFar / (fFar - fNear);
		matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
		matProj.m[2][3] = 1.0f;
		matProj.m[3][3] = 0.0f;

		while (engine::IsRunning())
		{
			if (!engine::ProcessEvents())
			{
				break;
			}
			engine::BeginFrame();
			
			colorBuffer::Clear(40, 100, 255);
			
	/*		for (uint16_t x = 0; x < colorBuffer::GetWidth(); x++)
			{
				for (uint16_t y = 0; y < colorBuffer::GetHeight(); y++)
				{
					colorBuffer::SetPixel(x, y,
						static_cast<uint8_t>(dis(gen)),
						static_cast<uint8_t>(dis(gen)),
						static_cast<uint8_t>(dis(gen)),
						255);
				}
			}*/

			Mat4 matRotZ, matRotX;
			fTheta += 1.0f * engine::GetDeltaTime();

			// Rotation Z
			matRotZ.m[0][0] = cosf(fTheta);
			matRotZ.m[0][1] = sinf(fTheta);
			matRotZ.m[1][0] = -sinf(fTheta);
			matRotZ.m[1][1] = cosf(fTheta);
			matRotZ.m[2][2] = 1.0f;
			matRotZ.m[3][3] = 1.0f;

			// Rotation X
			matRotX.m[0][0] = 1.0f;
			matRotX.m[1][1] = cosf(fTheta * 0.5f);
			matRotX.m[1][2] = sinf(fTheta * 0.5f);
			matRotX.m[2][1] = -sinf(fTheta * 0.5f);
			matRotX.m[2][2] = cosf(fTheta * 0.5f);
			matRotX.m[3][3] = 1.0f;

			// draw triangles
			for (const auto& tri : cubeMesh.triangles)
			{
				// Rotate in Z-Axis
				Triangle triRotatedZ;
				triRotatedZ.p[0] = matRotZ * tri.p[0];
				triRotatedZ.p[1] = matRotZ * tri.p[1];
				triRotatedZ.p[2] = matRotZ * tri.p[2];

				// Rotate in X-Axis
				Triangle triRotatedZX;
				triRotatedZX.p[0] = matRotX * triRotatedZ.p[0];
				triRotatedZX.p[1] = matRotX * triRotatedZ.p[1];
				triRotatedZX.p[2] = matRotX * triRotatedZ.p[2];

				// Offset into the screen
				Triangle triTranslated = triRotatedZX;
				triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
				triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
				triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;

				// Project triangles from 3D --> 2D
				Triangle triProjected;
				triProjected.p[0] = matProj * triTranslated.p[0];
				triProjected.p[1] = matProj * triTranslated.p[1];
				triProjected.p[2] = matProj * triTranslated.p[2];

				// Scale into view
				// triProjected работает в пространсве -1 до 1
				// сдвигаем к 0 до 2
				triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
				triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
				triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;
				// делим на 2 и умножаем на размер буфер
				triProjected.p[0].x *= 0.5f * (float)colorBuffer::GetWidth(); 
				triProjected.p[0].y *= 0.5f * (float)colorBuffer::GetHeight();
				triProjected.p[1].x *= 0.5f * (float)colorBuffer::GetWidth();
				triProjected.p[1].y *= 0.5f * (float)colorBuffer::GetHeight();
				triProjected.p[2].x *= 0.5f * (float)colorBuffer::GetWidth();
				triProjected.p[2].y *= 0.5f * (float)colorBuffer::GetHeight();

				// Rasterize triangle
				colorBuffer::DrawTriangle(
					triProjected.p[0].x, triProjected.p[0].y,
					triProjected.p[1].x, triProjected.p[1].y,
					triProjected.p[2].x, triProjected.p[2].y,
					255, 255, 255);
			}

			colorBuffer::SetPixel(1, 1, 255, 20, 20);

			colorBuffer::DrawLine(2, 2, 60, 50, 255, 100, 10);

			engine::EndFrame();
		}
	}
	engine::Shutdown();
}
//=============================================================================