#include "Raster.hpp"

Raster::Raster()
	: Raster{ 0, 0 }
{
}

Raster::Raster(int width, int height)
	: Raster{ width, height, Color{ 0, 0, 0, 255 } }
{
}

Raster::Raster(int width, int height, Color color)
	: width{ width }, height{ height }
{
	data.resize(4 * width * height);
	clear(color);
}

void Raster::clear(Color color)
{
	for (int i = 0; i < data.size(); i += 4)
	{
		data.at(i + 0) = color.r;
		data.at(i + 1) = color.g;
		data.at(i + 2) = color.b;
		data.at(i + 3) = color.a;
	}
}

void Raster::loadFromBuffer(const uint8_t* buffer)
{
	for (int i = 0; i < data.size(); i++)
		data.at(i) = buffer[i];
}

int Raster::getWidth() const
{
	return width;
}

int Raster::getHeight() const
{
	return height;
}

const std::vector<uint8_t>& Raster::getData() const
{
	return data;
}
