#include "stdafx.h"
#if defined(_WIN32)
#include "ColorBuffer.h"
#include "MiniOpenGL11.h"
#include "Core.h"
//=============================================================================
namespace
{
	GLuint colorBufferTextureID{ 0 };
	GLuint pboID{ 0 };
	uint8_t* shadowPtrBuffer{ nullptr }; // TODO: переделать под uint32_t

	uint16_t colorBufferWidth;
	uint16_t colorBufferHeight;
}
//=============================================================================
void colorBuffer::Create(uint16_t width, uint16_t height)
{
	colorBufferWidth = width;
	colorBufferHeight = height;

	shadowPtrBuffer = new uint8_t[width * height * 4];

	glGenTextures(1, &colorBufferTextureID);
	glBindTexture(GL_TEXTURE_2D, colorBufferTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenBuffers(1, &pboID);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboID);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, width * height * 4, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}
//=============================================================================
void colorBuffer::Destroy()
{
	if (shadowPtrBuffer) delete[] shadowPtrBuffer;
	if (pboID) glDeleteBuffers(1, &pboID);
	if (colorBufferTextureID) glDeleteTextures(1, &colorBufferTextureID);

	shadowPtrBuffer = nullptr;
	pboID = 0;
	colorBufferTextureID = 0;
}
//=============================================================================
void colorBuffer::Resize(uint16_t width, uint16_t height)
{
	if (colorBufferWidth == width && colorBufferHeight == height) return;

	Print("Color Buffer Resize: " + std::to_string(width) + "," + std::to_string(height));
	Destroy();
	Create(width, height);
}
//=============================================================================
uint16_t colorBuffer::GetWidth()
{
	return colorBufferWidth;
}
//=============================================================================
uint16_t colorBuffer::GetHeight()
{
	return colorBufferHeight;
}
//=============================================================================
void colorBuffer::BeginDraw()
{
	glBindTexture(GL_TEXTURE_2D, colorBufferTextureID);
}
//=============================================================================
void colorBuffer::EndDraw()
{
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboID);
	void* ptr = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
	if (ptr)
	{
		uint8_t* ptrBuffer = static_cast<uint8_t*>(ptr);
		memmove(ptr, (void*)shadowPtrBuffer, colorBufferWidth * colorBufferHeight * 4);		
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

		glBindTexture(GL_TEXTURE_2D, colorBufferTextureID);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, colorBufferWidth, colorBufferHeight, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	}

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}
//=============================================================================
void colorBuffer::Clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	for (uint16_t x = 0; x < colorBuffer::GetWidth(); x++)
	{
		for (uint16_t y = 0; y < colorBuffer::GetHeight(); y++)
		{
			SetPixel(x, y, r, g, b, a);
		}
	}
}
//=============================================================================
void colorBuffer::SetPixel(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	if (x >= colorBufferWidth || y >= colorBufferHeight) return;

	const size_t index = (y * colorBufferWidth + x) * 4u;
	shadowPtrBuffer[index + 0] = r;
	shadowPtrBuffer[index + 1] = g;
	shadowPtrBuffer[index + 2] = b;
	shadowPtrBuffer[index + 3] = a;
}
//=============================================================================
void colorBuffer::DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
	dx = x2 - x1; dy = y2 - y1;
	dx1 = abs(dx); dy1 = abs(dy);
	px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;
	if (dy1 <= dx1)
	{
		if (dx >= 0)
		{
			x = x1; y = y1; xe = x2;
		}
		else
		{
			x = x2; y = y2; xe = x1;
		}

		SetPixel(x, y, r, g, b, a);

		for (i = 0; x < xe; i++)
		{
			x = x + 1;
			if (px < 0)
				px = px + 2 * dy1;
			else
			{
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) y = y + 1; else y = y - 1;
				px = px + 2 * (dy1 - dx1);
			}
			SetPixel(x, y, r, g, b, a);
		}
	}
	else
	{
		if (dy >= 0)
		{
			x = x1; y = y1; ye = y2;
		}
		else
		{
			x = x2; y = y2; ye = y1;
		}

		SetPixel(x, y, r, g, b, a);

		for (i = 0; y < ye; i++)
		{
			y = y + 1;
			if (py <= 0)
				py = py + 2 * dx1;
			else
			{
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) x = x + 1; else x = x - 1;
				py = py + 2 * (dx1 - dy1);
			}
			SetPixel(x, y, r, g, b, a);
		}
	}
}
//=============================================================================
void colorBuffer::DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	DrawLine(x1, y1, x2, y2, r, g, b, a);
	DrawLine(x2, y2, x3, y3, r, g, b, a);
	DrawLine(x3, y3, x1, y1, r, g, b, a);
}
//=============================================================================
#endif // _WIN32