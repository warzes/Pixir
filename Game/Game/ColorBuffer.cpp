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
	uint8_t* ptrBuffer{ nullptr };

	uint16_t colorBufferWidth;
	uint16_t colorBufferHeight;
}
//=============================================================================
void colorBuffer::Create(uint16_t width, uint16_t height)
{
	colorBufferWidth = width;
	colorBufferHeight = height;

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
	if (pboID) glDeleteBuffers(1, &pboID);
	if (colorBufferTextureID) glDeleteTextures(1, &colorBufferTextureID);
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
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboID);
	void* ptr = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
	ptrBuffer = static_cast<uint8_t*>(ptr);
}
//=============================================================================
void colorBuffer::EndDraw()
{
	if (ptrBuffer)
	{
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
		ptrBuffer = nullptr;

		glBindTexture(GL_TEXTURE_2D, colorBufferTextureID);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, colorBufferWidth, colorBufferHeight, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	}

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}
//=============================================================================
void colorBuffer::SetPixel(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	if (x >= colorBufferWidth || y >= colorBufferHeight) return;

	const size_t index = (y * colorBufferWidth + x) * 4u;
	if (ptrBuffer)
	{
		ptrBuffer[index + 0] = r;
		ptrBuffer[index + 1] = g;
		ptrBuffer[index + 2] = b;
		ptrBuffer[index + 3] = a;
	}
}
//=============================================================================
#endif // _WIN32