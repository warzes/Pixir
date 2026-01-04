#pragma once

#if defined(_WIN32)

namespace colorBuffer
{
	void Create(uint16_t width, uint16_t height);
	void Destroy();

	void Resize(uint16_t width, uint16_t height);

	uint16_t GetWidth();
	uint16_t GetHeight();

	void BeginDraw();
	void EndDraw();

	void Clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
	void SetPixel(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

} // namespace colorBuffer

#endif // _WIN32