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
void colorBuffer::FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	auto drawline = [&](int sx, int ex, int ny) { for (int i = sx; i <= ex; i++) SetPixel(i, ny, r, g, b, a); };

	int t1x, t2x, y, minx, maxx, t1xp, t2xp;
	bool changed1 = false;
	bool changed2 = false;
	int signx1, signx2, dx1, dy1, dx2, dy2;
	int e1, e2;
	// Sort vertices
	if (y1 > y2) { std::swap(y1, y2); std::swap(x1, x2); }
	if (y1 > y3) { std::swap(y1, y3); std::swap(x1, x3); }
	if (y2 > y3) { std::swap(y2, y3); std::swap(x2, x3); }

	t1x = t2x = x1; y = y1;   // Starting points
	dx1 = (int)(x2 - x1); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
	else signx1 = 1;
	dy1 = (int)(y2 - y1);

	dx2 = (int)(x3 - x1); if (dx2 < 0) { dx2 = -dx2; signx2 = -1; }
	else signx2 = 1;
	dy2 = (int)(y3 - y1);

	if (dy1 > dx1) {   // swap values
		std::swap(dx1, dy1);
		changed1 = true;
	}
	if (dy2 > dx2) {   // swap values
		std::swap(dy2, dx2);
		changed2 = true;
	}

	e2 = (int)(dx2 >> 1);
	// Flat top, just process the second half
	if (y1 == y2) goto next;
	e1 = (int)(dx1 >> 1);

	for (int i = 0; i < dx1;) {
		t1xp = 0; t2xp = 0;
		if (t1x < t2x) { minx = t1x; maxx = t2x; }
		else { minx = t2x; maxx = t1x; }
		// process first line until y value is about to change
		while (i < dx1) {
			i++;
			e1 += dy1;
			while (e1 >= dx1) {
				e1 -= dx1;
				if (changed1) t1xp = signx1;//t1x += signx1;
				else          goto next1;
			}
			if (changed1) break;
			else t1x += signx1;
		}
		// Move line
	next1:
		// process second line until y value is about to change
		while (1) {
			e2 += dy2;
			while (e2 >= dx2) {
				e2 -= dx2;
				if (changed2) t2xp = signx2;//t2x += signx2;
				else          goto next2;
			}
			if (changed2)     break;
			else              t2x += signx2;
		}
	next2:
		if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
		if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
		drawline(minx, maxx, y);    // Draw line from min to max points found on the y
		// Now increase y
		if (!changed1) t1x += signx1;
		t1x += t1xp;
		if (!changed2) t2x += signx2;
		t2x += t2xp;
		y += 1;
		if (y == y2) break;

	}
next:
	// Second half
	dx1 = (int)(x3 - x2); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
	else signx1 = 1;
	dy1 = (int)(y3 - y2);
	t1x = x2;

	if (dy1 > dx1) {   // swap values
		std::swap(dy1, dx1);
		changed1 = true;
	}
	else changed1 = false;

	e1 = (int)(dx1 >> 1);

	for (int i = 0; i <= dx1; i++) {
		t1xp = 0; t2xp = 0;
		if (t1x < t2x) { minx = t1x; maxx = t2x; }
		else { minx = t2x; maxx = t1x; }
		// process first line until y value is about to change
		while (i < dx1) {
			e1 += dy1;
			while (e1 >= dx1) {
				e1 -= dx1;
				if (changed1) { t1xp = signx1; break; }//t1x += signx1;
				else          goto next3;
			}
			if (changed1) break;
			else   	   	  t1x += signx1;
			if (i < dx1) i++;
		}
	next3:
		// process second line until y value is about to change
		while (t2x != x3) {
			e2 += dy2;
			while (e2 >= dx2) {
				e2 -= dx2;
				if (changed2) t2xp = signx2;
				else          goto next4;
			}
			if (changed2)     break;
			else              t2x += signx2;
		}
	next4:

		if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
		if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
		drawline(minx, maxx, y);
		if (!changed1) t1x += signx1;
		t1x += t1xp;
		if (!changed2) t2x += signx2;
		t2x += t2xp;
		y += 1;
		if (y > y3) return;
	}
}
//=============================================================================
#endif // _WIN32