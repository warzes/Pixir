пример пиксельного шума

std::random_device rd;
std::mt19937 gen{ rd() };
std::uniform_int_distribution<> dis{ 0, 255 };

engine::BeginFrame();			
for (size_t x = 0; x < colorBuffer::GetWidth(); x++)
{
	for (size_t y = 0; y < colorBuffer::GetHeight(); y++)
	{
		colorBuffer::SetPixel(x, y,
			static_cast<uint8_t>(dis(gen)),
			static_cast<uint8_t>(dis(gen)),
			static_cast<uint8_t>(dis(gen)),
			255);
	}
}
engine::EndFrame();