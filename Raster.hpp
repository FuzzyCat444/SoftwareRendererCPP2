#ifndef RASTER_HPP
#define RASTER_HPP

#include <cstdint>
#include <vector>

#include <cstring>

struct Color
{
    Color() : Color{ 0, 0, 0, 255 } {}

    Color(int r, int g, int b, int a)
        : r{ r }, g{ g }, b{ b }, a{ a } {}

    void limit()
    {
        if (r > 255) r = 255;
        if (g > 255) g = 255;
        if (b > 255) b = 255;
        if (a > 255) a = 255;
    }

	int r, g, b, a;
};

class Raster
{
public:
	Raster();
	Raster(int width, int height);
	Raster(int width, int height, Color color);

	void clear();

	void setPixel(int x, int y, const Color& color)
    {
        int index = getIndex(x, y);

        uint8_t* pixelPtr = &data.at(index);
        *pixelPtr = (uint8_t)color.r;
        pixelPtr++;
        *pixelPtr = (uint8_t)color.g;
        pixelPtr++;
        *pixelPtr = (uint8_t)color.b;
        pixelPtr++;
        *pixelPtr = (uint8_t)color.a;
    }
	void setPixel(int index, const Color& color)
	{
        uint8_t* pixelPtr = &data.at(index);
        *pixelPtr = (uint8_t)color.r;
        pixelPtr++;
        *pixelPtr = (uint8_t)color.g;
        pixelPtr++;
        *pixelPtr = (uint8_t)color.b;
        pixelPtr++;
        *pixelPtr = (uint8_t)color.a;
	}
	Color getPixel(int x, int y) const
    {
        int index = getIndex(x, y);
        return Color{ data.at(index + 0), data.at(index + 1), data.at(index + 2), data.at(index + 3) };
    }
    Color getPixel(int index) const
    {
        return Color{ data.at(index + 0), data.at(index + 1), data.at(index + 2), data.at(index + 3) };
    }
    int getIndex(int x, int y) const
    {
        return (x + y * width) << 2;
    }

	void loadFromBuffer(const uint8_t* buffer);

	int getWidth() const;
	int getHeight() const;
	const std::vector<uint8_t>& getData() const;
private:
	int width;
	int height;
	std::vector<uint8_t> data;
};

#endif
