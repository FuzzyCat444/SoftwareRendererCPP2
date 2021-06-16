#ifndef RASTER_HPP
#define RASTER_HPP

#include <cstdint>
#include <vector>

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

	void clear(Color color);

	void setPixel(int x, int y, Color color)
    {
        int index = getIndex(x, y);
        if (!checkIndex(index))
            return;
        data.at(index + 0) = color.r;
        data.at(index + 1) = color.g;
        data.at(index + 2) = color.b;
        data.at(index + 3) = color.a;
    }
	void setPixel(int index, Color color)
	{
	    if (!checkIndex(index))
            return;
	    data.at(index + 0) = color.r;
	    data.at(index + 1) = color.g;
	    data.at(index + 2) = color.b;
	    data.at(index + 3) = color.a;
	}
	Color getPixel(int x, int y) const
    {
        int index = getIndex(x, y);
        if (!checkIndex(index))
            return Color{ 0, 0, 0, 255 };
        return Color{ data.at(index + 0), data.at(index + 1), data.at(index + 2), data.at(index + 3) };
    }
    Color getPixel(int index) const
    {
        if (!checkIndex(index))
            return Color{ 0, 0, 0, 255 };
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

	bool checkIndex(int index) const
	{
	    return index >= 0 && index < data.size();
	}
};

#endif
