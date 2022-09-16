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
    : width{ width }, height{ height }, size(width * height * 4)
{
    data = new uint8_t[size];
    clear(color);
}

Raster::~Raster()
{
    delete[] data;
}

void Raster::clear(Color color)
{
    for (int i = 0; i < size; i += 4)
    {
        data[i + 0] = color.r;
        data[i + 1] = color.g;
        data[i + 2] = color.b;
        data[i + 3] = color.a;
    }
}

void Raster::loadFromBuffer(const uint8_t* buffer)
{
    for (int i = 0; i < size; i++)
        data[i] = buffer[i];
}

int Raster::getWidth() const
{
    return width;
}

int Raster::getHeight() const
{
    return height;
}

const uint8_t* Raster::getData() const
{
    return data;
}

int Raster::getSize() const
{
    return size;
}